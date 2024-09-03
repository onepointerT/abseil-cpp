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
// File: path.h
// -----------------------------------------------------------------------------
//
// This header file contains an abstraction for real paths on a filesystem.
//
// The following abstractions are defined:
//
//   * Operations on permissions and a file's or directory's status and type.
//   * Copying and renaming a path and checking, if it exists.
//   * For custom path objects, you can inherit from `absl::Path` and
//     add functionality.
//
 

#ifndef ABSL_FILES_PATH_H_
#define ABSL_FILES_PATH_H_

#include <filesystem>

#include "absl/base/config.h"
#include "absl/fs/pathlike.h"

namespace absl {
ABSL_NAMESPACE_BEGIN

// The file's status, that has permissions and information on the file's or directory's type.
// (https://en.cppreference.com/w/cpp/filesystem/file_status)
using file_status = std::filesystem::file_status;
// The file's or directory's type. (https://en.cppreference.com/w/cpp/filesystem/file_type)
using file_type = std::filesystem::file_type;
// The permissions of a file or a directory. (https://en.cppreference.com/w/cpp/filesystem/perms)
using permissions = std::filesystem::perms;


// The class that represents a real existent path and can do operations
// on the filesystem itself.
class Path
    :   public PathLike
{
 public:
  Path( const std::string p );
  Path( const Path& p );

  // Check, if a file or a directory exists at this path's location.
  bool exists() const;

  // Copy the current path to another directory or, if it is a file, it is
  // also possible to copy it a file's location.
  bool copy( PathLike& to_target ) const;
  // Rename the data file or directory at the current path's location to pl.
  void rename( PathLike& pl );
  
  // Get the `file_status` consisting of permissions and the `file_type` at the current path.
  const file_status status() const;
  // Get the `permissions` for the current path.
  permissions get_permissions() const;
  // Set the `permissions` for the current path.
  bool set_permissions( permissions perms );

  // Check, if this is a regular file.
  bool is_regular_file() const;
  // Check, if this is a directory.
  bool is_directory() const;
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_FILES_PATH_H_
