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

#include "absl/icmp/icmp_uuid.h"

#include "absl/strings/str_replace.h"



namespace absl {


const std::basic_string<char> IcmpUUID::next_uuid() const noexcept {
    if ( m_last_instertion.length() > 0 ) {
        size_t pos = inserter_text.find(m_last_instertion.data());
        size_t pos_end = pos+36;
        size_t pos_next = inserter_text.find("|_", pos_end) + 2;
        return inserter_text.substr(pos_next, 36);
    }
    return "";
}


IcmpUUID::IcmpUUID( const std::basic_string<char> template_text, const UUID other )
    :   absl::UUID( other.empty() ? UUID(absl::UUID::generate()) : other )
    ,   inserter_text( template_text )
    ,   replacement_text( template_text )
{}

std::pair<std::pair<size_t, size_t>, const unsigned int> IcmpUUID::find_inserter_end( const UUID uuid ) const {
    UUID ins_id;
    if ( uuid.length() == 0 ) {
        ins_id = uuid;
    } else {
        ins_id = UUID::generate();
    }
    unsigned int inserter_start = 0;
    unsigned int inserter_count = 0;
    size_t pos_start = 0;
    size_t pos_end = 0;
    bool found = false;
    for ( unsigned int p = 0; p < size(); p++ ) {
        const std::string pos = replacement_text.substr( p, 2 );
        if ( pos.compare("|_") == 0 && replacement_text.substr(p+3, ins_id.length()).compare(ins_id) == 0 ) {
            ++inserter_start;
            ++inserter_count;
            pos_start = p+2;
            found = true;
        } else if ( pos.compare("|_") == 0 ) {
            ++inserter_start;
            if ( found ) {
                ++inserter_count;
            }
        } else if ( pos.compare("_|") == 0 && inserter_start == 0 && found ) {
            pos_end = p;
            found = false;
            break;
        } else if ( pos.compare("_|") == 0 && found ) {
            --inserter_start;
        }
    }
    return std::make_pair(std::make_pair(pos_start, pos_end), inserter_count);
}


std::pair<size_t, size_t> IcmpUUID::find_inserter_pos( const UUID uuid ) const {
    return this->find_inserter_end( uuid ).first;
}

IcmpUUID* IcmpUUID::on_next_inserter_start( const std::string inserter_string ) {
    const std::string nextid = this->next_uuid();
    
    std::pair<std::pair<size_t,size_t>, const unsigned int> inserter_params
        = this->find_inserter_end( nextid );
    
    const std::string next_inserter_text = absl::StrReplaceAll( this->replacement_text,
            {{ replacement_text.substr( inserter_params.first.first,
                inserter_params.first.second - inserter_params.first.first),
               inserter_string
            }}
        );
    
    IcmpUUID* icmp_uuid = new IcmpUUID( next_inserter_text, nextid );
    return icmp_uuid;
}

unsigned int IcmpUUID::count_subuuids() {
    return this->find_inserter_end( UUID(UUID::substr()) ).second;
}

bool IcmpUUID::merge_into( const UUID& gapid, const std::basic_string<char>& text ) {

    replacement_text = absl::StrReplaceAll(replacement_text,
        { {gapid.data(), text} }
    );

    m_last_instertion = gapid.data();

    return true;
}

bool IcmpUUID::merge_into( const std::basic_string<char> text ) {
    
    replacement_text = absl::StrReplaceAll(replacement_text,
        { {data(), text} }
    );
    
    m_last_instertion = data();

    return true;
}


} // namespace absl
