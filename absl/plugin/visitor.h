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

#ifndef ABSL_PLUGIN_VISITOR_H_
#define ABSL_PLUGIN_VISITOR_H_

#include <map>
#include <queue>
#include <utility>

#include "absl/base/config.h"
#include "absl/flags/flag_queue.h"
#include "absl/strings/string_view.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


// A plugin visitor. Can reach plugins with some information
// from `property_flags_t` without changing a bit at the
// main program's/librarie's and the plugin's execution stack's
// state. Is normally given from `PluginAPI` to `PluginCompositor`,
// so that the plugin context only needs to decide, which strategy to start.
template< class property_flags_t >
class PluginVisitor
{
protected:
    property_flags_t* m_flags;
    
    virtual void swap(  property_flags_t* flags ) final {
        m_flags = flags;
    }
public:
    PluginVisitor()
        :   m_flags( new property_flags_t() )
    {}

    virtual property_flags_t* visit() { return this->m_flags; }
    virtual bool visited( property_flags_t* property ) {
        this->m_flags = property;
        return true;
    }

    virtual void reset() final {
        this->swap( new property_flags_t() );
    }
};





// A plugin visitor with queue. Can reach plugins with some information
// from `property_flags_t` without changing a bit at the
// main program's/librarie's and the plugin's execution stack's
// state. Is normally given from `PluginAPI` to `PluginCompositor`,
// so that the plugin context only needs to decide, which strategy to start.
template< typename property_flags_t >
class PluginVisitorQueue
    :   public PluginVisitor< property_flags_t >
    ,   public FlagQueue< property_flags_t >
{
public:
    PluginVisitorQueue();

    // Please call `visited()` when the visitor finished visiting completly.
    virtual property_flags_t* property_visit( property_flags_t* flags = new property_flags_t() ) {
        this->push( flags );
        this->m_flags = flags;
        return PluginVisitor< property_flags_t >::visit();
    }

};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_PLUGIN_VISITOR_H_
