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
// This header file contains a class to inherit from and implement everything
// a plugin needs for its plugin strategy.
//

#ifndef ABSL_PLUGIN_STRATEGY_H_
#define ABSL_PLUGIN_STRATEGY_H_



#include "absl/base/config.h"
#include "absl/plugin/visitor.h"


namespace absl {
ABSL_NAMESPACE_BEGIN


// A template plugin strategy that can be started without
// touching anything at runtime (at least at the main library/
// program) or only operate on the plugin's context.
template< typename property_flags_t >
class PluginStrategy {
    // Start the strategy with a visitor or hand in the context aswell.
    virtual static bool start( PluginVisitor< property_flags_t >* visitor
                             , PluginContext< property_flags_t >* ctx = nullptr
    ) {
        if ( ctx != nullptr ) {
            return false;
        }
        return false;
    }
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_PLUGIN_STRATEGY_H_
