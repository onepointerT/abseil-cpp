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
// This header file contains 
//
// The following abstractions are defined:
//
//   * 
//   * 
//   * 
//
 

#ifndef ABSL_FILES_PATH_H_
#define ABSL_FILES_PATH_H_

#include <filesystem>

#include "absl/base/config.h"
#include "absl/fs/pathlike.h"

namespace absl {
ABSL_NAMESPACE_BEGIN

using file_status = std::filesystem::file_status;
using file_type = std::filesystem::file_type;
using permissions = std::filesystem::perms;


class Path
    :   public PathLike
{
 public:
  Path( const std::string p );
  Path( const Path& p );

  bool exists() const;

  bool copy( PathLike& to_target ) const;
  void rename( PathLike& pl );
  
  const file_status status() const;
  permissions get_permissions() const;
  bool set_permissions( permissions perms );

  bool is_regular_file() const;
  bool is_directory() const;
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_FILES_PATH_H_
