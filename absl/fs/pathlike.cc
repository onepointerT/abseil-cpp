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

#include "absl/fs/pathlike.h"
#include "absl/strings/str_replace.h"
#include "absl/strings/str_split.h"


namespace absl {
ABSL_NAMESPACE_BEGIN

PathLike::PathLike( const std::string path )
    :   absl::string_view( path )
{}


PathLike::PathLike( const PathLike& other )
    :   absl::string_view( other.substr() )
{}


void PathLike::join( const std::string pjoin ) {
    absl::string_view p( this->repr() + '/' + pjoin.data() );
    this->swap( p );
}


void PathLike::operator/( const std::string pjoin ) {
    this->join( pjoin );
}


PathLike::path_elements_t PathLike::path_elements() const {
    return absl::StrSplit( this->data(), absl::ByAnyChar("/") );
}


std::filesystem::path PathLike::path() const {
    std::filesystem::path p( this->repr() );
    return p;
}


const std::string PathLike::repr() const {
#ifdef _MSC_BUILD 
    return absl::StrReplaceAll( *this, {{"/", "\\"}} );
#else
    return this->data();
#endif
}


bool PathLike::valid() const {
    return PathLike::is_valid_path(*this);
}


bool PathLike::is_valid_path( const PathLike& p ) {
#ifdef _MSC_BUILD
    return p[1] == ':' ? true : p.contains("$") == 0;
    // Labeled drive or network mount count as valid path.
#else
    return p[0] == '/' ? true : p.contains(":///") == 0;
    // Unix path or POSIX-like path like smb:/// or file:///
#endif
}


bool PathLike::is_valid_path( const std::string& p ) {
    PathLike pl( p );
    return PathLike::is_valid_path( pl );
}


PathLike operator/( const PathLike& lhs, const PathLike& rhs ) {
    PathLike pl( lhs );
    pl.join( rhs.data() );
    return pl;
}


PathLike operator/( const PathLike& lhs, const std::string& rhs ) {
    PathLike pl( lhs );
    pl.join( rhs );
    return pl;
}


PathLike operator/( const std::string& lhs, const std::string& rhs ) {
    PathLike pl( lhs );
    pl.join( rhs );
    return pl;
}


ABSL_NAMESPACE_END
}  // namespace absl