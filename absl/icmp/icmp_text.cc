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

#include "absl/icmp/icmp_text.h"

#include <algorithm>

#include "absl/icmp/icmp_inlining.h"
#include "absl/icmp/icmp_strategies.h"
#include "absl/strings/str_replace.h"



namespace absl {


bool IcmpText::operate_impl() {
    size_t operated = m_gaps.size();
    typename std::vector< UUID* >::reverse_iterator it = m_gaps.rbegin();
    unsigned int success = 0;
    while ( operated != 0 ) {
        const UUID* gap_id = *it;
        IcmpProperties* property = nullptr;
        for ( PluginContextQueue< IcmpProperties >::iterator pIt 
                    = PluginContextQueue< IcmpProperties >::begin()
            ; pIt != PluginContextQueue< IcmpProperties >::end()
            ; ++pIt
        ) {
            const IcmpProperties* ctx_property = *pIt;
            if ( ctx_property->gap_id->compare(gap_id->substr()) == 0 ) {
                // TODO: property = PluginContextQueue< IcmpProperties >::;
                break;
            }
        }
        if ( this->fill_gap(property) ) {
            ++success;
            --operated;
        }
    }
    return success > 0;
}


IcmpText::IcmpText( const std::string text )
    :   IcmpStringView( text )
    ,   absl::PluginContextQueue< IcmpProperties >("icmp_text")
    ,   m_visitor()
    ,   m_gaps()
    ,   own_id()
{}


bool IcmpText::fill_gap( const IcmpProperties* property ) {
    return this->fill_gap( *property->gap_id, absl::string_view(property->gap_text.data()) );
}


bool IcmpText::fill_gap( const UUID gapid, const absl::string_view gaptext ) {

    std::string* s = new std::string(this->substr());

    absl::StrReplaceAll( {{ gapid.substr().data(), gaptext }}, s );
    absl::string_view* sv = new absl::string_view( s->c_str() );
    absl::string_view::swap( *sv );
    return true;
}

const std::string IcmpText::text() const {
    return absl::string_view::data();
}


IcmpProperties* IcmpText::get_new_properties() {
    UUID* gapid = new UUID();
    IcmpProperties* property = new IcmpProperties( own_id, gapid->substr() );
    this->push( property );
    m_gaps.push_back( gapid );
    return property;
}


IcmpProperties* IcmpText::getPropertyByID( const UUID propertyid ) {
    for ( typename PluginContextQueue< IcmpProperties >::iterator it
                = PluginContextQueue< IcmpProperties >::begin()
        ; it != PluginContextQueue< IcmpProperties >::end(); ++it
    ) {
        const IcmpProperties* prop = *it;
        if ( prop->gap_id->compare(propertyid.substr()) == 0 ) {
            return *it; // Syntax (*&)(it) -> Auswertung &((*)it)
        }
    }
    return nullptr;
}

const absl::PluginVisitor< IcmpProperties >& IcmpText::visitor() const {
    return m_visitor;
}

IcmpProperties* IcmpText::property_visit( IcmpProperties* property ) {
    property->gap_text.parentText = this;
    property->gap_text.parentProperty = m_flags;

    // IMPL property.gap_text.parentStr

    property->gap_text.visited( property->gap_text.visit() );

    this->push( property );
    return m_visitor.property_visit( property );
}


bool IcmpText::visited( IcmpProperties* property ) {
    return fill_gap( property );
}


IcmpText::operator std::basic_string<char>() const {
    return this->own_id;
}



IcmpTextSet::IcmpTextSet()
    :   std::vector< IcmpText* >()
    ,   absl::Singleton< IcmpTextSet >()
{}



IcmpText* IcmpTextSet::get_new_text( const std::string text ) {
    IcmpText* icmp_text = new IcmpText( text );
    push_back( icmp_text );
    return icmp_text;
}


IcmpText* IcmpTextSet::getTextByID( const UUID textid ) {
    for ( unsigned int i = 0; i < size(); i++ ) {
        IcmpText* it = at(i);
        if ( it->own_id.compare(textid) == 0 ) {
            return it;
        }
    }
    return nullptr;
}


IcmpText* IcmpTextSet::getTextByPropertyID( const UUID propertyid ) {
    for ( unsigned int i = 0; i < size(); i++ ) {
        IcmpText* it = at(i);
        if ( it->getPropertyByID( propertyid ) != nullptr ) {
            return it;
        }
    }
    return nullptr;
}


IcmpProperties* IcmpTextSet::getPropertyByID( const UUID propertyid ) {
    for ( unsigned int i = 0; i < size(); i++ ) {
        IcmpText* it = at(i);
        IcmpProperties* property = it->getPropertyByID( propertyid );
        if ( property != nullptr ) {
            return property;
        }
    }
    return nullptr;
}


IcmpProperties* IcmpTextSet::getPropertyByID( const UUID textid, const UUID propertyid ) {
    IcmpText* icmp_text = getTextByID( textid );
    if ( icmp_text != nullptr ) {
        IcmpProperties* property = icmp_text->getPropertyByID( propertyid );
        return property;
    }
    return nullptr;
}


} // namespace absl
