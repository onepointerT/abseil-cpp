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

#ifndef ABSL_ICMP_ICMP_CONCLUDANT_H_
#define ABSL_ICMP_ICMP_CONCLUDANT_H_

#include <cstddef>
#include <set>

#include "absl/base/config.h"
#include "absl/icmp/icmp_inlining.h"
#include "absl/icmp/icmp_peering.h"
#include "absl/icmp/icmp_strategy.h"
#include "absl/icmp/icmp_text.h"
#include "absl/plugin/api.h"
#include "absl/plugin/context.h"
#include "absl/plugin/strategy.h"


namespace absl {
ABSL_NAMESPACE_BEGIN

template< class peer_t >
class IcmpConcludant;


class IcmpAPI
    :   PluginContext< IcmpProperties >
{
public:
    IcmpAPI( const std::string plugin_name )
        :   PluginContext< IcmpProperties >( plugin_name )
    {}

    bool is_plugin( const std::string plugin_name );

    template< class peer_t >
    friend class IcmpConcludant;
};


template< class peer_t = IcmpPeer >
class IcmpConcludant final
    :   public PluginAPI< peer_t >
    ,   public absl::Singleton< IcmpConcludant< peer_t > >
{
private:
    static const absl::string_view ms_guess;

protected:
    peer_t* m_peer;
    IcmpPeering< peer_t >* m_peering;


    
public:
    IcmpConcludant()
        :   PluginAPI< peer_t >(new IcmpPeering< peer_t >())
        ,   absl::Singleton< IcmpConcludant< peer_t > >()
        ,   m_peering( (IcmpPeering<peer_t>*)this->plugin_api_ctx )
    {}

    IcmpStrategy* strategy( const std::string plugin_name ) const;
    IcmpStrategizerMap* strategies() const;
    
    void make_peer_unique( peer_t* peer );

    const absl::string_view inform_guesser(); // TODO: peer.guess;
    
    void hand_in( const absl::string_view guess );

};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_ICMP_ICMP_MY_PEER_H_
