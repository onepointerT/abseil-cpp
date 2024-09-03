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

#include <fstream>
#include <string>

#include "absl/fs/file.h"


namespace absl {
ABSL_NAMESPACE_BEGIN


File::File( const std::string p )
    :   Path( p )
{}


file_size File::fsize() const {
    return std::filesystem::file_size( this->repr() );
}


bool File::resize( const file_size size_bytes_new ) const {
    std::error_code ec;
    std::filesystem::resize_file( this->repr(), size_bytes_new, ec );
    return !ec;
}


const absl::string_view File::read(
  file_openmode mode  ) const {
    std::string s;
    std::fstream fstr( this->repr(), std::ios_base::in | mode );
    fstr >> s;
    fstr.close();
    return absl::string_view( s );
}


void File::write( const absl::string_view s,
  file_openmode mode ) const {
    std::fstream fstr( this->repr(), std::ios_base::out | mode );
    fstr << s << '\0';
    fstr.close();
}


File& File::operator<<( const absl::string_view in ) {
  this->write( in, std::ios_base::app );
  return *this;
}


File& File::operator>>( std::string out ) {
  out.append( this->read( std::ios_base::in ) );
  return *this;
}


ABSL_NAMESPACE_END
}  // namespace absl