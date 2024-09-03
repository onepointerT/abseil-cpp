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
// This header file contains everything you need to comfortly handle directories
// even without using iterators.
//
// The following abstractions are defined:
//
//   * `absl::DirectoryContent`: A bivariate class that stores one of the known 
//     abseil filesystem classes that can appear in a directory. Error-safe.
//   * `absl::DirectoryContentList`: A sorted list of generic paths, files and
//     directories, that is able to separate them by class type and sort them by path.
//   * `absl::Directory`: A inheritor of `absl::Path` that also scans on its real directory's
//     contents on the filesystem and presents it in a content list.
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



// Polymorphic and bivariate directory content class, that is either a `absl::File`,
// a `absl::Directory` or a generic `absl::Path`.
class DirectoryContent
    :   protected absl::container_internal::CompressedTuple< int, File*, Directory*, Path* >
    ,   protected std::string
{
 public:
    // The Type of the stored directory content. Harmonic to `absl::DirectoryContent::get_pos()`.
    enum class Type : int {
        File = 1,
        Directory = 2,
        GenericPath = 3
    };

    // Construct `absl::DirectoryContent` from a `absl::Directory`.
    DirectoryContent( Directory& dir );
    // Construct `absl::DirectoryContent` from a `absl::File`.
    DirectoryContent( File& file );
    // Construct `absl::DirectoryContent` from a generic `absl::Path`.
    DirectoryContent( Path& path );
    // Construct `absl::DirectoryContent` from another `absl::DirectoryContent`.
    DirectoryContent( const DirectoryContent& dc );

    const std::string pathstr() const;
    // Get this `absl::DirectoryContent` as `absl::Path` object.
    const Path path() const;

    // Get a pointer to the `absl::Directory`, if not a `nullptr`.
    Directory* get_directory() const;
    // Get a pointer to the `absl::File`, if not a `nullptr`.
    File* get_file() const;
    // Get a pointer to the `absl::Path` or convert to.
    Path* get_path() const;
    const int get_pos() const;
    
    // Check, which type of directory content is stored or use a switch-case on
    // `DirectoryContent::Type` or compate `get_^* != nullptr` with the `get_..`
    // functions above.
    bool is( const DirectoryContent::Type dct ) const;

    // Used for sorting in `std::set< T >`.
    constexpr auto operator<=>(const DirectoryContent& rhs) const;
    // Used for sorting in `std::set< T* >`.
    constexpr auto operator<=>(const DirectoryContent*rhs) const;
    // Used for key comparator in `std::set< T >`.
    operator std::basic_string<char, std::char_traits<char>, std::allocator<char>>() const;
};


// A sorted `std::set< DirectoryContent >` that is able to seperate between files and directories.
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

    // Get only all files as a `absl::DirectoryContentList`.
    DirectoryContentList get_files() const;
    // Get only all files as a `absl::DirectoryContentList`.
    DirectoryContentList get_directories() const;
};


// A directory. Stores all content in a compatible sorted `absl::DirectoryContentList`
// that can be accessed to with `absl::Directory::contents` and fills it with the
// result of C++17's iterators (`absl::DirectoryContent(*it)`) on the directories path.
// `absl::Directory` can be inherited from.
class Directory
    :   public Path
{
 protected:
    DirectoryContentList m_contents;

 public:
    // Get all directory content like gotten from C++17's
    // `std::filesystem::recursive_directory_iterator` as a fully-featured sorted set of
    // bivariate object's of the type `absl::DirectoryContent`.
    static DirectoryContentList scan_directory( const Path& p, const bool recursive = false );

    Directory( const std::string p );

    // Update the sorted contents set with the current directory contents and get the number of items.
    // One may call `absl::Directory::contents()` when called this function.
    size_t scandir( const bool recursive = false );
    // Get the sorted-by-path set of unique generic or specific path objects.
    const DirectoryContentList contents() const;
};


ABSL_NAMESPACE_END
}  // namespace absl


#endif  // ABSL_FILES_DIRECTORY_H_
