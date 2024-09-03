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

#include "absl/fs/path.h"



namespace absl {
ABSL_NAMESPACE_BEGIN


Path::Path( const std::string p )
    :   PathLike( p )
{}


Path::Path( const Path& p )
    :   PathLike( p )
{}


bool Path::exists() const {
    return std::filesystem::exists( this->repr() );
}


bool Path::copy( PathLike& to_target ) const {
    std::error_code ec;
    std::filesystem::copy( this->repr(), to_target.repr(), ec );
    return !ec;
}


void Path::rename( PathLike& pl ) {
    std::filesystem::rename( this->repr(), pl.repr() );
    this->swap( pl );
}


const file_status Path::status() const {
    return std::filesystem::status( this->repr() );
}


permissions Path::get_permissions() const {
    return this->status().permissions();
}


bool Path::set_permissions( permissions perms ) {
    std::error_code ec;
    std::filesystem::permissions( this->repr(), perms, ec );
    return !ec;
}


bool Path::is_regular_file() const {
    return std::filesystem::is_regular_file( this->status() );
}


bool Path::is_directory() const {
    return std::filesystem::is_directory( this->status() );
}


ABSL_NAMESPACE_END
}  // namespace absl