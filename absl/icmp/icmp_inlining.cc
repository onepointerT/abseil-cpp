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

#include "absl/icmp/icmp_inlining.h"

#include "absl/icmp/icmp_concludant.h"
#include "absl/icmp/icmp_mypeer.h"
#include "absl/icmp/icmp_text.h"
#include "absl/strings/string_view.h"


namespace absl {


IcmpProperties::IcmpProperties( const UUID textid
                    , const UUID gapid
                    , const std::basic_string<char> inserter_text
)   :   text_id( new UUID(textid) )
    ,   gap_id( new IcmpUUID(gapid.data()) )
    ,   gap_text(inserter_text)
{}


void IcmpProperties::swap_text( const std::basic_string<char> text ) {
    gap_text = IcmpStringView( text );
    gap_text.parentProperty = this;
    gap_text.parentStr = new IcmpStringView(gap_id->replacement_text);
    gap_text.parentText = new IcmpStringView(
        IcmpTextSet::get()->getTextByPropertyID(*gap_id)->text()
    );
}


void IcmpProperties::fill_gap() {
    IcmpText* icmp_text = IcmpTextSet::get()->getTextByID( text_id->substr() );
    if ( icmp_text->fill_gap( *gap_id, gap_id->replacement_text.data() ) ) {
        status = Status::FilledGapInText;
    }
}


IcmpProperties* IcmpProperties::visit() {
    switch ( status ) {
    case Status::New: {
        const absl::string_view sv
            = IcmpConcludant<>::get()->inform_guesser();

        if ( sv.length() > 0 ) {
            gap_text.replaceAll( sv );
            status = Status::Guessed;
        }
        break;
        
    } case Status::Guessed: {
        
        IcmpPeer* peer = new IcmpPeer();

        // Guess merge type
        

        switch ( gap_type ) {
        case IcmpGapType::Single:
            status = Status::MergedMyGap;
            break;
        
        case IcmpGapType::Merge:
            status = Status::MergedMyGap;
            break;

        case IcmpGapType::MergeAll:
            status = Status::MergedMyGap;
            break;

        default:
            break;
        }

        break;
    } case Status::MergedMyGap: {
        // Fill gap into text or push queue
        if ( gap_text.parentText != nullptr ) {
            fill_gap();
        }
        break;

    } case Status::FilledGapInText: {
        IcmpText* icmp_text = IcmpTextSet::get()->getTextByID( text_id->substr() );
        icmp_text->erase_gapid( text_id->substr() );
        break;

    } default: {
        break;
    }
    }

    gap_text.visited( this );
    return this;
}



} // namespace absl
