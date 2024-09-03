// Copyright 2024 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -----------------------------------------------------------------------------
// File: directory.h
// -----------------------------------------------------------------------------
//
// This header file contains 
//
// The following abstractions are defined:
//
//   * 
//   * 
//   * 
//
// 
//
// References:
//
//  https://yaml.org/spec/1.2.2/
//  

#ifndef ABSL_FILES_DIRECTORY_H_
#define ABSL_FILES_DIRECTORY_H_

#include <set>

#include "absl/base/config.h"
#include "absl/container/compressed_tuple.h"
#include "absl/fs/file.h"


namespace absl {
ABSL_NAMESPACE_BEGIN

class Directory;




class DirectoryContent
    :   protected absl::container_internal::CompressedTuple< int, File*, Directory*, Path* >
    ,   protected std::string
{
 public:

    DirectoryContent( Directory& dir );
    DirectoryContent( File& file );
    DirectoryContent( Path& path );
    DirectoryContent( const DirectoryContent& dc );

    const std::string pathstr() const;
    const Path path() const;

    Directory* get_directory() const;
    File* get_file() const;
    Path* get_path() const;
    const int get_pos() const;
    
    constexpr auto operator<=>(const DirectoryContent& rhs) const {
        return this->path().repr() <=> rhs.path().repr();
    }
    
    constexpr auto operator<=>(const DirectoryContent*rhs) const {
        return this->path().repr() <=> rhs->path().repr();
    }

    operator std::basic_string<char, std::char_traits<char>, std::allocator<char>>() const {
        return this->path().repr();
    }

};



class DirectoryContentList
    :   public std::set< DirectoryContent >
{
 public:
    typedef typename std::set< DirectoryContent > BaseDirectoryContentList;

    using BaseDirectoryContentList::emplace;
    using BaseDirectoryContentList::insert;
    using BaseDirectoryContentList::size;
    using BaseDirectoryContentList::iterator;
    using BaseDirectoryContentList::const_iterator;
    using BaseDirectoryContentList::begin;
    using BaseDirectoryContentList::end;
    using BaseDirectoryContentList::cbegin;
    using BaseDirectoryContentList::cend;

    DirectoryContentList();
    DirectoryContentList( const DirectoryContentList& dcl );

    DirectoryContentList get_files() const;
    DirectoryContentList get_directories() const;
};

class Directory
    :   public Path
{
 protected:
    DirectoryContentList m_contents;

 public:
    static DirectoryContentList scan_directory( const Path& p, const bool recursive = false );

    Directory( const std::string p );

    size_t scandir( const bool recursive = false );
    const DirectoryContentList contents() const;
};


ABSL_NAMESPACE_END
}  // namespace absl


#endif  // ABSL_FILES_DIRECTORY_H_
