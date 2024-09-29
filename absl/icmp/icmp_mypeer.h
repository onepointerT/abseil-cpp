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

#ifndef ABSL_ICMP_ICMP_MY_PEER_H_
#define ABSL_ICMP_ICMP_MY_PEER_H_

#include <future>
#include <set>

#include "absl/base/config.h"
#include "absl/flags/flag_queue.h"
#include "absl/icmp/icmp_peering.h"
#include "absl/icmp/icmp_inlining.h"
#include "absl/icmp/icmp_text.h"
#include "absl/plugin/visitor.h"


namespace absl {
ABSL_NAMESPACE_BEGIN

//class IcmpProperties;
/* Forward-declaration */
/*template< class peer_t >
class IcmpPeering;*/
/*template< class peer_t, class peering_t >
class IcmpPeeringStrategizer;*/


class IcmpPeer
    :   public FlagQueue< IcmpProperties >
    ,   public absl::PluginVisitor< IcmpProperties >
{
protected:
    static std::future< const absl::string_view > await_answer( IcmpProperties* awaiter );
    
public:
    const UUID myID;

    IcmpPeer();

    /* IMPL only for peer derivers */

    virtual IcmpProperties* getPropertyByTextID( const UUID& textid ) final;
    virtual IcmpProperties* getPropertyByGuessID( const UUID& guessid ) final;

    virtual const absl::string_view guess( const UUID guessid );

    /* IMPL declarations, pure-virtual or virtual, however for friend class IcmpPeering */
    friend class IcmpPeering< IcmpPeer >;

    // What are no-gos for friend class PeeringStrategyzer< MYCLASS >.
    // What are need-to function definitions for a friend class Strategizer-for-my-Peer-base-class.
    // Which of them need to be virtual or none-virtual.
    //friend class IcmpPeeringStrategizer< IcmpPeer, IcmpPeering< IcmpPeer > >;
};




ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_ICMP_ICMP_MY_PEER_H_
