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
#include "absl/plugin/visitor.h"
#include "absl/strings/string_view.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


// A basic Plugin class that can hold more than one other named class
// of this or derived type and is the elementar base class for plugins.
class PluginBase
    :   protected std::map< const absl::string_view, PluginBase* >  // tuples (plugin_name, plugin)
{

public:
    // Constructor.
    PluginBase();

    // Get a specific plugin. Returns (false, nullptr) only, when not found.
    std::pair< bool, PluginBase* > get_plugin( const std::string name ) const;
    // Register a plugin.
    bool add_plugin( const std::string plugin_name, PluginBase* plugin );
};


// Inherit from this class to start a plugin.
// Loading a plugin is possible with `PluginCompositor( PluginAPI_ )`.
// Holds a pointer to the main plugin api of the main library
// that is somehow giving some and less (at least please all needed)
// information on its context to each plugin that is registered.
// Operate on a `PluginContext< property_flags_t >`.
template< typename property_flags_t >
class PluginCompositor
    :   public PluginBase
{
protected:
    const PluginBase* m_myLib_im_plugged_into;
    PluginContext< property_flags_t >* m_plugin_context;

public:
    // Constructor.
    PluginCompositor( const PluginBase* plug_me_inside_this_lib )
        :   m_myLib_im_plugged_into( plug_me_inside_this_lib )
        ,   m_plugin_context( nullptr )
    {}
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_PLUGIN_COMPOSITOR_H_
