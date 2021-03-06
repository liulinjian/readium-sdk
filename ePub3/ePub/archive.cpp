//
//  archive.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
//  Copyright (c) 2012-2013 The Readium Foundation.
//  
//  The Readium SDK is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "archive.h"
#include "zip_archive.h"
#include <map>

EPUB3_BEGIN_NAMESPACE

Archive::ArchiveRegistrationDomain Archive::RegistrationDomain;

void Archive::RegisterArchive(ArchiveTypeSniffer sniffer, ArchiveFactory factory)
{
    RegistrationDomain[sniffer] = factory;
}
void Archive::Initialize()
{
    RegisterArchive([](const std::string& path) { return path.rfind(".zip") == path.size()-4; },
                    [](const std::string& path) { return new ZipArchive(path); });
    RegisterArchive([](const std::string& path) { return path.rfind(".epub") == path.size()-5; },
                    [](const std::string& path) { return new ZipArchive(path); });
}
Archive * Archive::Open(const std::string& path)
{
    for ( auto item : RegistrationDomain )
    {
        if ( item.first(path) )
            return item.second(path);
    }
    
    return nullptr;
}
bool Archive::ShouldCompress(const std::string &path, const std::string &mimeType, size_t size) const
{
    // check MIME type for known pre-compressed data formats
    if ( mimeType.find("image/", 0, 6) != std::string::npos )
        return false;
    if ( mimeType.find("video/", 0, 6) != std::string::npos )
        return false;
    if ( mimeType.find("audio/", 0, 6) != std::string::npos )
        return false;
    
    // Under 1KB don't bother compressing anything
    if ( size < 1024 )
        return false;
    
    return true;
}
ArchiveItemInfo Archive::InfoAtPath(const std::string &path) const
{
    ArchiveItemInfo info;
    info.SetPath(path);
    return std::move(info);
}

EPUB3_END_NAMESPACE
