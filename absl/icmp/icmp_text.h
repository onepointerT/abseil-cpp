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

#ifndef ABSL_ICMP_ICMP_TEXT_H_
#define ABSL_ICMP_ICMP_TEXT_H_

#include <set>
#include <string>
#include <utility>
#include <vector>

#include "absl/base/config.h"
#include "absl/base/singleton.h"
#include "absl/base/uuid.h"
#include "absl/flags/flag_queue.h"
//#include "absl/icmp/icmp_inlining.h"
#include "absl/icmp/icmp_string_view.h"
#include "absl/plugin/context.h"
#include "absl/plugin/visitor.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


class IcmpProperties;
class IcmpStrategyDefault;


class IcmpText final
    :   public IcmpStringView
    ,   public absl::PluginContextQueue< IcmpProperties >
{
protected:
    absl::PluginVisitorQueue< IcmpProperties > m_visitor;
    absl::FlagQueue< UUID > m_gaps;

    bool operate_impl();

    virtual bool operate_stratgy( const IcmpStrategyDefault& strategy );

public:
    UUID own_id;

    IcmpText( const std::string text = "" );

    bool erase_gapid( const UUID& gapid );

    bool fill_gap( const IcmpProperties* property );
    bool fill_gap( const UUID gapid, const absl::string_view gaptext );

    const std::string text() const;
    IcmpProperties* get_new_properties();
    IcmpProperties* getPropertyByID( const UUID propertyid );

    const absl::PluginVisitor< IcmpProperties >& visitor() const;

    // Only inherit and implement, if you implement own strategies for merging text
    IcmpProperties* property_visit( IcmpProperties* property );
    bool visited( IcmpProperties* property );

    operator std::basic_string<char>() const;
};



class IcmpTextSet
    :   public std::vector< IcmpText* >
    ,   public absl::Singleton< IcmpTextSet >
{
protected:
    typedef typename std::vector< IcmpText* > Vector;
    
    IcmpTextSet();

    friend class Singleton< IcmpTextSet >;

public:
    using Vector::iterator;
    using Vector::end;

    IcmpText* get_new_text( const std::string text = "" );

    IcmpText* getTextByID( const UUID textid );
    IcmpText* getTextByPropertyID( const UUID propertyid );
    IcmpProperties* getPropertyByID( const UUID propertyid );
    IcmpProperties* getPropertyByID( const UUID textid, const UUID propertyid );
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_ICMP_ICMP_TEXT_H_
