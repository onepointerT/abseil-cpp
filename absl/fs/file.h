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
// This header file contains an implementation for a generic file.
//
// The following additional abstractions are defined:
//
//   * Inspecting the file's size and changing it.
//   * Reading/writing to and from the file (with and without `<<` and `>>`).
//   * All operations from `absl::Path`.
//

#ifndef ABSL_FILES_FILE_H_
#define ABSL_FILES_FILE_H_



#include "absl/base/config.h"
#include "absl/fs/path.h"

namespace absl {
ABSL_NAMESPACE_BEGIN

// The default type of file's size
typedef std::uintmax_t file_size;
// The openmodes available for reading and writing files.
// (https://en.cppreference.com/w/cpp/io/ios_base/openmode)
typedef typename std::ios_base::openmode file_openmode;


// The class that handles most file objects on filesystem.
// It is simply a `absl::Path` with functions on file's size and
// functions for read and write.
// It can be inherited from, e.g. for socket or other file types.
class File
    :   public Path
{
 public:
  File( const std::string p );
  
  // Get the file's size on filesystem.
  file_size fsize() const;
  // Resize the file to another size.
  bool resize( const file_size size_bytes_new ) const;

  // Read from the file and obtain a `string_view`.
  virtual const absl::string_view read(
    file_openmode mode = std::ios_base::in ) const;
  // Write to the file on filesystem.
  virtual void write( const absl::string_view s,
    file_openmode mode = std::ios_base::app | std::ios_base::out ) const;

  // Write to the file on filesystem with a stream operator.
  File& operator<<( const absl::string_view in );
  // Read from the file and obtain an appendation to a `std::string`.
  File& operator>>( std::string out );
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_FILES_FILE_H_
