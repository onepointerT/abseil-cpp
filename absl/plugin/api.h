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
#include <utility>

#include "absl/base/config.h"
#include "absl/plugin/compositor.h"
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
    :   public PluginBase
    ,   public PluginVisitor< property_flags_t >
{
protected:
    // The plugin context of the current library.
    PluginContext< property_flags_t >* plugin_ctx;

public:
    // The name of this plugin api.
    const absl::string_view api_name;

    // The name of your plug-inable implementation
    // of the plugin-api and its compositor's strategies.
    PluginAPI( const std::string name )
        :   
    {}

    // Load from config, directory, ld-loaded library or similar.
    virtual unsigned int load_plugins() = 0;

    // Send the plugin api to operate on the plugin context.
    virtual bool operate() const {
        return plugin_ctx->operate( this );
    }
};



ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_PLUGIN_API_H_
