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
// File: pathlike.h
// -----------------------------------------------------------------------------
//
// This header file contains the abstract and elementar data type path-like.
//
// The following abstractions are defined:
//
//   * Concatenating strings and path-like with /   :=  PathLike pl = "C:" / "repositories"
//   * Representations of paths per operating system:=  Windows with '\\', Unix with '/'
//   * Further introspection on path elements       :=  "C:/User/My" -> {"C:", "User", "My"}
//   * 

#ifndef ABSL_FILES_PATHLIKE_H_
#define ABSL_FILES_PATHLIKE_H_

#include <filesystem>
#include <vector>

#include "absl/base/config.h"
#include "absl/strings/string_view.h"


namespace absl {
ABSL_NAMESPACE_BEGIN


class PathLike
    :   public absl::string_view
{
 public:
  // Elements between the path seperators like '/' and '\\' in an vector.
  typedef typename std::vector< std::string > path_elements_t;

  PathLike( const std::string path );
  PathLike( const PathLike& other );

  // Append some letters to the current path. It makes
  //  * this + '/' + pjoin
  void join( const std::string pjoin );
  // Append some letters to the current path-like. It makes
  //  * this->join(pjoin)
  void operator/( const std::string pjoin );

  // Get a vector with only every directory element. The last element
  // is either a directory with files or a file itself.
  path_elements_t path_elements() const;

  std::filesystem::path path() const;

  // Gives the representation for the current operating system as string
  const std::string repr() const;

  // Returns, if it is a valid path on the current operating system
  bool valid() const;
  // Returns, if it is a valid path on the current operating system
  static bool is_valid_path( const PathLike& p );
  // Returns, if it is a valid path on the current operating system
  static bool is_valid_path( const std::string& p );
};


PathLike operator/( const PathLike& lhs, const PathLike& rhs );
PathLike operator/( const PathLike& lhs, const std::string& rhs );
PathLike operator/( const std::string& lhs, const std::string& rhs );


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_FILES_PATHLIKE_H_
