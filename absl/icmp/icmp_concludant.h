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
#include "absl/icmp/icmp_text.h"
#include "absl/plugin/api.h"


namespace absl {
ABSL_NAMESPACE_BEGIN

template< class peer_t >
class IcmpConcludant;


class IcmpAPI final {
public:
    bool plugin();

    template< class peer_t >
    friend class IcmpConcludant;
};


template< class peer_t = IcmpPeer >
class IcmpConcludant
    :   public PluginAPI< IcmpProperties >
    ,   public absl::Singleton< IcmpConcludant< peer_t > >
{
private:
    static const absl::string_view ms_guess;

protected:
    peer_t* m_peer;


    
public:
    IcmpConcludant();
    //virtual ~IcmpConcludant() = delete;
    // TODO/IMPL: Plugin own strategies and choose them
    //bool plugin( const IcmpPeeringStrategy< peer_t >* peering_strategy_plugin );
    virtual unsigned int load_plugins() { return 0; }

    void make_peer_unique( peer_t* peer );

    const absl::string_view inform_guesser(); // TODO: peer.guess;
    
    void hand_in( const absl::string_view guess );

};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_ICMP_ICMP_MY_PEER_H_
