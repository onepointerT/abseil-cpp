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

#ifndef ABSL_PLUGIN_PEERS_H_
#define ABSL_PLUGIN_PEERS_H_

#include "absl/base/config.h"
#include "absl/base/uuid.h"
#include "absl/plugin/context.h"
#include "absl/plugin/informant.h"
#include "absl/plugin/visitor.h"


namespace absl {
ABSL_NAMESPACE_BEGIN


template< typename property_flags_t >
class PluginAPI;
template< typename property_flags_t >
class PluginPeer;


template< typename property_flags_t >
class PluginPeering
    :   public PluginContextQueue< property_flags_t >
{
public:
    friend class PluginPeer< property_flags_t >;
    friend class PluginAPI< property_flags_t >;

protected:
    virtual property_flags_t* let_peer_answer( property_flags_t* property
                                , PluginPeer< property_flags_t >* peer ) {
        std::future< property_flags_t > future
            = std::async(std::launch::async, [property, peer]{
                peer->guess( "peer_answer", property );

            });
        future.wait();
        return future.get();
    }

    virtual property_flags_t* inform_me( property_flags_t* property ) {
        std::promise< property_flags_t > promise;
        std::future< property_flags_t > future = promise.get_future();
        std::thread thread([this,&promise, property]{
            promise.set_value_at_thread_exit(0);
            this->let_peer_answer( property, new PluginPeer< property_flags_t >() )
        }).detach();
        
        future.wait();

        return future.get();
    }
};


template< typename property_flags_t >
class PluginPeer
    :   public PluginInformant< property_flags_t >
    ,   public PluginVisitor< property_flags_t >
{
public:
    PluginPeer();

    virtual property_flags_t* guess( const std::string strategy_name
                                   , property_flags_t* property
    ) {
        std::future< property_flags_t > future
            = this->await_information( property, strategy_name );
        future.wait();
        if ( ! future.valid() ) {
            return nullptr;
        }
        return future.get();
    }

    virtual bool visited_peers( property_flags_t* property, PluginPeering< PluginPeer< property_flags_t > >* peering ) {
        property_flags_t* property_flags = this->guess( property );
        if ( property_flags != nullptr ) {
            peering->inform_me( property_flags );
            return true;
        }
        return false;
    }
};




ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_PLUGIN_PEERS_H_
