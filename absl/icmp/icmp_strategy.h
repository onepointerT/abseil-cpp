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

#ifndef ABSL_ICMP_ICMP_STRATEGY_H_
#define ABSL_ICMP_ICMP_STRATEGY_H_

#include <map>

#include "absl/base/config.h"
#include "absl/base/singleton.h"
#include "absl/icmp/icmp_inlining.h"
#include "absl/plugin/context.h"
#include "absl/plugin/strategy.h"
#include "absl/strings/string_view.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


template< class DerivedT >
class IcmpStrategy
    :   public PluginStrategy< IcmpProperties >
{
protected:


public:
    IcmpStrategy()
        :   PluginStrategy< IcmpProperties >()
    {}

    static bool start( PluginVisitor< IcmpProperties >* visitor
                     , PluginContext< IcmpProperties >* ctx
    ) {
        if ( visitor == nullptr || ctx == nullptr ) {
            return false;
        }

        return DerivedT::start( visitor, ctx );
    }
};



class IcmpStrategizerMap final
    :   public std::map< absl::string_view, PluginStrategy< IcmpProperties >* >
    ,   public PluginContext< IcmpProperties >
    ,   public Singleton< IcmpStrategizerMap >
{
protected:


    bool operate_impl( const std::string strategy_name );

    using PluginContext< IcmpProperties >::operate;
    using PluginContext< IcmpProperties >::operate_impl;

public:
    
    explicit IcmpStrategizerMap();
    //virtual ~IcmpStrategizerMap() = delete;
    
    bool register_strategy( const std::string strategy_name, PluginStrategy< IcmpProperties >* strategy );
    bool operate( PluginVisitor< IcmpProperties >* visitor
                , const std::string strategy_name );
    
    using std::map< absl::string_view, PluginStrategy< IcmpProperties >* >::operator[];
    using std::map< absl::string_view, PluginStrategy< IcmpProperties >* >::clear;
    using std::map< absl::string_view, PluginStrategy< IcmpProperties >* >::size;
};


//#define icmps IcmpStrategizerMap::get()
//#define icmps_register IcmpStrategizerMap::get()->register
//#define icmps_register_strategy(sname, strategy) IcmpStrategizerMap::get()->operator[sname] = strategy;
//#define icmps_register_strategy(sname) IcmpStrategizerMap::get()->operator[sname]



ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_ICMP_ICMP_STRATEGY_H_
