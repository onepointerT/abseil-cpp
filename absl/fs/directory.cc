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

#include "absl/fs/directory.h"


namespace absl {
ABSL_NAMESPACE_BEGIN


DirectoryContentList Directory::scan_directory( const Path& p, const bool recursive ) {
    DirectoryContentList dircontent;
    for ( const std::filesystem::directory_entry& direntry
            : std::filesystem::recursive_directory_iterator( p.repr() )
    ) {
        if ( direntry.is_directory() ) {
            Directory* dir = new Directory( direntry.path().string() );
            dircontent.insert( DirectoryContent( *dir ) );
            if ( recursive ) {
                DirectoryContentList recursive_dircontent
                    = Directory::scan_directory( dir->path().string(), true );
                dircontent.insert_range( recursive_dircontent );
            }
        } else if ( direntry.is_regular_file() ) {
            File* file = new File( direntry.path().string() );
            dircontent.insert( DirectoryContent( *file ) );
        } else {
            Path* path = new Path( direntry.path().string() );
            dircontent.insert( DirectoryContent( *path ) );
        }
    }
    return dircontent;
}


Directory::Directory( const std::string p )
    :   Path( p )
    ,   m_contents()
{}


size_t Directory::scandir( const bool recursive ) {
    m_contents = Directory::scan_directory( *this, recursive );
    return m_contents.size();
}


const DirectoryContentList Directory::contents() const {
    return m_contents;
}


DirectoryContentList::DirectoryContentList()
    :   std::set< DirectoryContent >()
{}



DirectoryContentList::DirectoryContentList( const DirectoryContentList& dcl )
    :   std::set< DirectoryContent >( dcl )
{}


DirectoryContentList DirectoryContentList::get_files() const {
    DirectoryContentList dircontent_files;
    for ( const_iterator it = this->cbegin(); it != this->cend(); ++it ) {
        if ( it->get_file() != nullptr ) {
            dircontent_files.insert( *it );
        }
    }
    return dircontent_files;
}


DirectoryContentList DirectoryContentList::get_directories() const {
    DirectoryContentList dircontent_dirs;
    for ( const_iterator it = this->cbegin(); it != this->cend(); ++it ) {
        if ( it->get_directory() != nullptr ) {
            dircontent_dirs.insert( *it );
        }
    }
    return dircontent_dirs;
}


DirectoryContent::DirectoryContent( Directory& dir )
    :   absl::CompressedTuple< int, File*, Directory*, Path* >( 1, nullptr, &dir, nullptr )
    ,   std::string( dir.repr() )
{}


DirectoryContent::DirectoryContent( File& file )
    :   absl::CompressedTuple< int, File*, Directory*, Path* >( 0, &file, nullptr, nullptr )
    ,   std::string( file.repr() )
{}


DirectoryContent::DirectoryContent( Path& path )
    :   absl::CompressedTuple< int, File*, Directory*, Path* >( 2, nullptr, nullptr, &path )
    ,   std::string( path.repr() )
{}


DirectoryContent::DirectoryContent( const DirectoryContent& dc )
    :   absl::CompressedTuple< int, File*, Directory*, Path* >( dc.get_pos(), dc.get_file(), dc.get_directory(), new Path(dc.pathstr()) )
    ,   std::string( dc.path().repr() )
{}

const std::string DirectoryContent::pathstr() const {
    return substr();
}


const Path DirectoryContent::path() const {
    return Path( pathstr() );
}


Directory* DirectoryContent::get_directory() const {
    return this->get<2>();
}

File* DirectoryContent::get_file() const {
    return this->get<1>();
}

Path* DirectoryContent::get_path() const {
    switch ( this->get<0>() ) {
        case 0:
            return this->get_file();
        case 1:
            return this->get_directory();
        default:
            return this->get<3>();
    }
}


int DirectoryContent::get_pos() const {
    return this->get<0>();
}


bool DirectoryContent::is( const DirectoryContent::Type dct ) const {
    return static_cast<int>(dct) == this->get_pos();
}


DirectoryContent::Type DirectoryContent::is_type( const DirectoryContent& dc ) {
    if ( dc.get_pos() > 0 ) {
        return static_cast<DirectoryContent::Type>( dc.get_pos() );
    }
    return DirectoryContent::Type::GenericPath;
}

auto DirectoryContent::operator<=>(const DirectoryContent& rhs) const {
    return this->path().repr() <=> rhs.path().repr();
}

auto DirectoryContent::operator<=>(const DirectoryContent*rhs) const {
    return this->path().repr() <=> rhs->path().repr();
}

DirectoryContent::operator std::basic_string<char>() const {
    return this->path().repr();
}


ABSL_NAMESPACE_END
}  // namespace absl