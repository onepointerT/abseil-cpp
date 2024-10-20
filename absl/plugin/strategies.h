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

#ifndef ABSL_PLUGIN_STRATEGIES_H_
#define ABSL_PLUGIN_STRATEGIES_H_

#include <initializer_list>
#include <map>

#include "absl/base/config.h"
#include "absl/base/singleton.h"
#include "absl/plugin/context.h"
#include "absl/plugin/strategy.h"
#include "absl/plugin/visitor.h"
#include "absl/strings/string_view.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


template< class property_flags_t >
class PluginStrategizerMap
    :   public std::map< absl::string_view, PluginStrategy< property_flags_t >* >
    ,   public PluginContext< property_flags_t >
    ,   public Singleton< PluginStrategizerMap<property_flags_t> >
{
protected:


    bool operate_impl( const std::string strategy_name
                     , PluginVisitor< property_flags_t >* visitor
    ) {
        
        if ( this->at( strategy_name )->start( visitor, this ) ) {
            return PluginContext< property_flags_t >::operate_impl();
        }
        return false;
    }

    // Start to operate on and with a plugin with the plugin 
    // visitor from the main PluginAPI.
    // Please implement
    virtual bool operate( PluginVisitor< property_flags_t >* op_visitor
                        , std::initializer_list< std::string > strategies = {}
    ) {
        
        for ( auto it = strategies.begin(); it != strategies.end(); ++it ) {
            if ( ! this->operate_impl( *it, op_visitor ) ) {
                return false;
            }
        }

        return PluginContext< property_flags_t >::operate( visitor );
    }

    using PluginContext< property_flags_t >::operate;

    friend class PluginContext< property_flags_t >;

public:
    
    explicit PluginStrategizerMap();
    
    bool register_strategy( const std::string strategy_name, PluginStrategy< property_flags_t >* strategy );
    bool operate( PluginVisitor< property_flags_t >* visitor
                , const std::string strategy_name );
    
    using std::map< absl::string_view, PluginStrategy< property_flags_t >* >::operator[];
    using std::map< absl::string_view, PluginStrategy< property_flags_t >* >::at;
    using std::map< absl::string_view, PluginStrategy< property_flags_t >* >::clear;
    using std::map< absl::string_view, PluginStrategy< property_flags_t >* >::size;
};



ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_ICMP_ICMP_STRATEGY_H_
