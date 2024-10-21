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

#ifndef ABSL_ICMP_ICMP_STRING_VIEW_H_
#define ABSL_ICMP_ICMP_STRING_VIEW_H_

#include <string>

//#include "absl/icmp/icmp_inlining.h"
#include "absl/plugin/visitor.h"
#include "absl/strings/string_view.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


class IcmpProperties;


class IcmpStringView
    :   public absl::string_view
    ,   public PluginVisitor< IcmpProperties >
{
public:
    IcmpStringView* parentText;
    IcmpStringView* parentStr;
    IcmpProperties* parentProperty;


    IcmpStringView( const std::basic_string<char> text );

    void replace( const std::basic_string<char> substr
                , const std::basic_string<char> replacement
    );
    void replaceAll( const absl::string_view other );

    /* TODO: IMPL */
    //void set_parent_str( IcmpStringView* parent_str );

    //bool mergeAll();
    //bool mergeFilled();
    //const unsigned int delete_merge_gaps();

    virtual IcmpProperties* visit();
    virtual bool visited( IcmpProperties* property );

    operator std::basic_string<char>();
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_ICMP_ICMP_STRING_VIEW_H_