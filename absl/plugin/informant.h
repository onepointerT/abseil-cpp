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
// This header file contains the plugin visitor class.
//

#ifndef ABSL_PLUGIN_INFORMANT_H_
#define ABSL_PLUGIN_INFORMANT_H_

#include <future>
#include <map>
#include <string>

#include "absl/base/config.h"
#include "absl/plugin/strategy.h"
#include "absl/strings/string_view.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


template< typename property_flags_t >
class PluginAPI;
template< typename property_flags_t >
class PluginAPIStrategy;


template< class property_flags_t  >
class PluginInformant {
public:
    friend class PluginAPIStrategy< property_flags_t >;
    friend class PluginAPI< property_flags_t >;

protected:
    std::map< std::string, PluginAPIStrategy< property_flags_t >* > strategies;

    PluginAPIStrategy< property_flags_t >* get_strategy( const std::string sname ) {
        if ( this->strategies.contains(sname) ) {
            return this->strategies.at(sname);
        }
        return nullptr;
    }

    std::shared_future<property_flags_t*> await_information( property_flags_t* properties
                                                     , const std::string information_strategy ) {
        PluginAPIStrategy< property_flags_t >* api_strategy = this->get_strategy( information_strategy );
        if ( api_strategy == nullptr ) {
            return std::future< property_flags_t* >();
        }

        return std::async(std::launch::async
                    , [api_strategy, properties]{
                        return api_strategy->start(properties);
                    }
        ).share();
    }

public:
    void register_api_strategy( const std::string sname, PluginAPIStrategy< property_flags_t >* strategy ) {
        this->strategies[sname] = strategy;
    }

    void unregister_api_strategy( const std::string sname ) {
        this->strategies.erase(sname);
    }
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_PLUGIN_INFORMANT_H_
