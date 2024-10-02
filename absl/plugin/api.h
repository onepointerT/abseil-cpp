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
// This header file contains the `PluginAPI` class.
//

#ifndef ABSL_PLUGIN_API_H_
#define ABSL_PLUGIN_API_H_

#include <map>
#include <string>
#include <utility>

#include "absl/base/config.h"
#include "absl/plugin/compositor.h"
#include "absl/plugin/context.h"
#include "absl/plugin/informant.h"
#include "absl/plugin/visitor.h"
#include "absl/strings/string_view.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


// The PluginAPI main class.
// property_flags_t is a struct or class that holds all information
// that are allowed besides the main library or program and are allowed
// to be passed to plugins and their thread for syncronously operating
// with the plugins and the pointer to the specific plugin api,
// that is held by only the main library or the program.
template< typename property_flags_t >
class PluginAPI
    :   public PluginBase< property_flags_t >
    ,   public PluginVisitor< property_flags_t >
{
protected:
    typedef std::map< std::string, PluginCompositor< property_flags_t >* > plugin_map_t;

    // The plugin context of the current library.
    PluginContext< property_flags_t >* plugin_api_ctx;
    PluginInformant< property_flags_t >* plugin_informant;
    plugin_map_t plugin_map;

public:
    // The name of this plugin api.
    const absl::string_view api_name;

    // The name of your plug-inable implementation
    // of the plugin-api and its compositor's strategies.
    PluginAPI( const std::string name, PluginContext< property_flags_t > ctx = nullptr )
        :   PluginBase< property_flags_t >( ctx, name )
        ,   PluginVisitor< property_flags_t >()
        ,   plugin_api_ctx( new PluginContext< property_flags_t >(name) )
        ,   plugin_map()
    {}

    // Register a plugin implementation
    virtual bool plugin_add( const PluginContext< property_flags_t >* plugin_ctx ) {
        if ( plugin_ctx == nullptr ) {
            return false;
        }
        this->add_plugin( plugin_ctx );
        return true;
    }

    // Load from config, directory, loaded plugin library and its compositors or similar.
    virtual unsigned int load_plugins( std::vector< PluginContext< property_flags_t >* > plugins ) = 0 {
        unsigned int num_plugins = 0;
        for ( unsigned int ip = 0; ip < plugins.size(); ip++ ) {
            PluginContext< property_flags_t >* plugin_ctx
                = plugins.at(ip);
            PluginCompositor< property_flags_t >* plugin
                = new PluginCompositor< property_flags_t >( this, plugin_ctx );
            plugin_map[plugin_ctx->name_plugin] = plugin;
            ++num_plugins;
        }
        return num_plugins;
    }

    // Send the plugin api to operate on the plugin context.
    virtual bool operate() {
        return plugin_api_ctx->operate( this );
    }

    virtual std::shared_future< property_flags_t* > finish_inform(
          const std::string finish_strategy_name
        , property_flags_t* property
    ) {
        PluginAPIStrategy< property_flags_t > api_strategy
            = plugin_informant->get_strategy( finish_strategy_name );
        return std::async( std::launch::async
            , [property, api_strategy, plugin_api_ctx]{
                api_strategy->start( plugin_api_ctx, property )
            }
        ).share();
    }

    virtual std::shared_future<property_flags_t*> inform( const std::string strategy_name
                                                        , property_flags_t* property
    ) {
        // If informing other API plugins, do it pre `finish_inform()`-call

        std::shared_future< property_flags_t* > future
            = this->finish_inform( property, strategy_name );
        future.wait();
        if ( ! future.valid() ) {
            return std::shared_future<property_flags_t*>();
        }
        return future;
    }
};



ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_PLUGIN_API_H_
