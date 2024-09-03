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
// File: file.h
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

#ifndef ABSL_FILES_FILE_H_
#define ABSL_FILES_FILE_H_



#include "absl/base/config.h"
#include "absl/fs/path.h"

namespace absl {
ABSL_NAMESPACE_BEGIN

typedef std::uintmax_t file_size;
typedef typename std::ios_base::openmode file_openmode;


class File
    :   public Path
{
 public:
  File( const std::string p );
  
  file_size fsize() const;
  bool resize( const file_size size_bytes_new ) const;

  virtual const absl::string_view read(
    file_openmode mode = std::ios_base::in ) const;
  virtual void write( const absl::string_view s,
    file_openmode mode = std::ios_base::app | std::ios_base::out ) const;
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_FILES_FILE_H_
