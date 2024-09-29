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

#ifndef ABSL_PLUGIN_CONTEXT_H_
#define ABSL_PLUGIN_CONTEXT_H_

#include <type_traits>
#include <utility>

#include "absl/base/config.h"
#include "absl/flags/flag_queue.h"
#include "absl/plugin/visitor.h"
#include "absl/strings/string_view.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


template< typename property_flags_t >
class PluginCompositor;
template< typename property_flags_t >
class PluginAPI;
template< typename property_flag_t >
class PluginStrategy;


// A plugin context. Holds a visitor of the kind
// `PluginVisitor< property_flags_t >`, when visited and needs
// to implement in specific derived classes, how to send it to a
// specific plugin strategy and how to choose which strategy of
// a plugin to execute when the main plugin api (the visitor)
// arrives.
template< typename property_flags_t >
class PluginContext {
protected:
    PluginVisitor< property_flags_t >* visitor;

     // Choose strategy with visitor.
     // TODO: Implement virtual function.
    virtual bool operate_impl() {
        if ( this->visitor == nullptr ) {
            return false;
        }
        return false;
    }
    // Operate on a specific strategy with visitor.
    // TODO: Implement virtual function.
    template< typename strategy_properties_t >
    bool operate_strategy( const PluginStrategy< strategy_properties_t >* strategy ) const {
        return strategy->start( visitor, this );
    }

public:
    // Initialize a plugin context. The visitor will bring
    // every guess and properties that are allowed outside
    // of the main library inside a plugin. Please specify
    // these common parameters for your implementations.
    PluginContext()
        :   visitor( nullptr )
    {}

    // Start to operate on and with a plugin with the plugin 
    // visitor from the main PluginAPI.
    // Please implement
    virtual bool operate( PluginVisitor< property_flags_t >* op_visitor ) {
        this->visitor = op_visitor;
        return this->operate_impl();
    }

    friend class PluginCompositor< property_flags_t >;
    friend class PluginAPI< property_flags_t >;
};



template< typename property_flags_t >
class PluginContextQueue 
    :   public PluginContext< property_flags_t >
    ,   public FlagQueue< property_flags_t >
{
public:
    PluginContextQueue()
        :   PluginContext< property_flags_t >()
        ,   FlagQueue< property_flags_t >()
    {}

    // Start to operate on and with a plugin with the plugin 
    // visitor from the main PluginAPI.
    // Please implement
    virtual bool operate( PluginVisitorQueue< property_flags_t >* op_visitor ) {
        this->visitor = op_visitor;
        return this->operate_impl();
    }
    
    // Start to operate on and with a plugin with the plugin 
    // visitor from the main PluginAPI.
    // Please implement
    virtual bool operate( PluginVisitor< property_flags_t >* op_visitor ) {
        this->visitor = op_visitor;
        return this->operate_impl();
    }
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_PLUGIN_CONTEXT_H_
