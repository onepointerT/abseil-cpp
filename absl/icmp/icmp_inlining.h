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
// This header file contains all classes the plugin compositor and the
// plugin context need for the plugin base and the plugin-api-base.
// Please inherit from those classes here.
//

#ifndef ABSL_ICMP_ICMP_INLINING_H_
#define ABSL_ICMP_ICMP_INLINING_H_



#include "absl/base/config.h"
#include "absl/base/uuid.h"
//#include "absl/icmp/icmp_mypeer.h"
//#include "absl/icmp/icmp_peering.h"
#include "absl/icmp/icmp_string_view.h"
#include "absl/icmp/icmp_uuid.h"
#include "absl/plugin/compositor.h"
#include "absl/plugin/visitor.h"
#include "absl/strings/string_view.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


class IcmpText;
class IcmpStringView;


enum IcmpGapType {
    None,
    Single,
    Merge,
    MergeAll
};

class IcmpProperties
    :   public PluginVisitor< IcmpProperties >
{
public:
    enum Status {
        New,
        Guessed,
        MergedMyGap,
        FilledGapInText
    } status = Status::New;

    const UUID* text_id;
    const IcmpUUID* gap_id;
    IcmpStringView gap_text;
    IcmpGapType gap_type = IcmpGapType::Single;

    IcmpProperties( const UUID textid = UUID(UUID::generate())
        , const UUID gap_id = UUID(UUID::generate())
        , const std::basic_string<char> inserter_text = ""
    );

    void swap_text( const std::basic_string<char> text );
    void fill_gap();

    // Only inherit and implement, if you implement own strategies for merging text
    virtual IcmpProperties* visit();
};


/*const unsigned int icmp_inline_me( const IcmpProperties& property );
const unsigned int icmp_inline_me( const PluginVisitorQueue< IcmpProperties >& visitor_properties );

const unsigned int icmp_inline_me( const IcmpProperties& property, PluginContext< IcmpProperties >& ctx );
const unsigned int icmp_inline_me( const PluginVisitorQueue< IcmpProperties >& visitor_properties, PluginContext< IcmpProperties >& ctx );*/

ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_ICMP_ICMP_INLINING_H_
