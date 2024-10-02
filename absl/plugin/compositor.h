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

#ifndef ABSL_PLUGIN_COMPOSITOR_H_
#define ABSL_PLUGIN_COMPOSITOR_H_

#include <queue>
#include <type_traits>
#include <utility>

#include "absl/base/config.h"
#include "absl/plugin/context.h"
#include "absl/plugin/strategy.h"
#include "absl/plugin/visitor.h"
#include "absl/strings/string_view.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


template< typename property_flags_t >
class PluginContext;


// A basic Plugin class that can hold more than one other named class
// of this or derived type and is the elementar base class for plugins.
template< typename property_flags_t >
class PluginBase
    :   protected std::map< const absl::string_view, PluginBase< property_flags_t >* >  // tuples (plugin_name, plugin)
    ,   public PluginContext< property_flags_t >
{

public:
    // Constructor.
    PluginBase( PluginStrategy< property_flags_t >* plugin_strategy = nullptr
              , const std::string api_plugin_name = UUID::generate()
    )   :   std::map< const absl::string_view, PluginBase* >()
        ,   PluginContext< property_flags_t >( api_plugin_name, plugin_strategy )
    {}
    PluginBase( PluginContext< property_flags_t >* plugin_ctx_impl )
        :   std::map< const absl::string_view, PluginBase* >()
        ,   PluginContext< property_flags_t >( plugin_ctx_impl->name_plugin, plugin_ctx_impl->get_strategy() )
    {}

    // TODO: Load by config

    // Get a specific plugin. Returns (false, nullptr) only, when not found.
    std::pair< bool, PluginBase* > get_plugin( const std::string name ) const {
        bool is_there = this->contains(name);
        return std::make_pair(is_there, is_there ? this->at(name) : nullptr );
    }
    // Register a plugin.
    bool add_plugin( const std::string plugin_name, PluginBase* plugin ) {
        this->at(plugin_name) = plugin;
        return true;
    }
    bool add_plugin( PluginContext< property_flags_t >* plugin_ctx_impl ) {
        PluginBase< property_flags_t >* plugin_base
            = new PluginBase< property_flags_t >( plugin_ctx_impl );
        this->at(plugin_base->name_plugin) = plugin_base;
        return true;
    }
};


// Inherit from this class to start a plugin.
// Loading a plugin is possible with `PluginCompositor( PluginAPI_ )`.
// Holds a pointer to the main plugin api of the main library
// that is somehow giving some and less (at least please all needed)
// information on its context to each plugin that is registered.
// Operate on a `PluginContext< property_flags_t >`.
template< typename property_flags_t >
class PluginCompositor
    :   PluginAPIStrategy< property_flags_t >
{
protected:
    // The plugins can cooperate
    const PluginBase< property_flags_t >* m_myLib_iam_plugged_into;
    // Each plugin has a context
    PluginContext< property_flags_t >* m_plugin_context;
    // Each Plugin needs to implement a strategy

    friend class PluginContext< property_flags_t >;

public:
    // Constructor.
    PluginCompositor( const PluginBase< property_flags_t >* plug_me_inside_this_lib
                    , PluginContext< property_flags_t >* plugin_ctx
    )   :   PluginAPIStrategy< property_flags_t >()
        ,   m_myLib_iam_plugged_into( plug_me_inside_this_lib )
        ,   m_plugin_context( nullptr )
    {}

    virtual property_flags_t* start( property_flags_t* properties ) {
        // Do things with the plugin's base lib `m_myLib_iam_plugged_into`
        // Find plugin strategies and operate upon the context with the own properties
        // Finally call `operate` upon the plugin's own context
        if ( m_plugin_context->operate( properties ) ) {
            return properties;
        }
        return nullptr;
    }
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_PLUGIN_COMPOSITOR_H_
