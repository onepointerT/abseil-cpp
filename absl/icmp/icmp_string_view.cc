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

#include "absl/icmp/icmp_string_view.h"

#include "absl/icmp/icmp_inlining.h"
#include "absl/strings/str_replace.h"


namespace absl {


IcmpStringView::IcmpStringView( const std::basic_string<char> text )
    :   absl::string_view( text )
    ,   PluginVisitor< IcmpProperties >()
    ,   parentText( nullptr )
    ,   parentStr( nullptr )
    ,   parentProperty( nullptr )
{}

void IcmpStringView::replace( const std::basic_string<char> substr
            , const std::basic_string<char> replacement
) {
    std::string* orig = new std::string( this->substr() );
    absl::StrReplaceAll({ { substr, replacement } }, orig);
    absl::string_view* sv = new absl::string_view( orig->c_str() );
    string_view::swap( *sv );
}

//bool IcmpStringView::mergeAll();
//const unsigned int IcmpStringView::delete_merge_gaps();


IcmpProperties* IcmpStringView::visit() {

    if ( parentProperty != nullptr ) {
        parentProperty->visit();
        m_flags = parentProperty;
    } else {
        m_flags = parentProperty;
    }

    return m_flags;
}


bool IcmpStringView::visited( IcmpProperties* property ) {
    this->parentProperty = property;
    parentProperty->status = IcmpProperties::Status::New;
    return true;
}


IcmpStringView::operator std::basic_string<char>() {
    return this->data();
}

} // namespace absl
