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

#ifndef ABSL_PLUGIN_PROPERTY_H_
#define ABSL_PLUGIN_PROPERTY_H_



#include "absl/base/config.h"
#include "absl/base/uuid.h"
#include "absl/plugin/visitor.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


template< class DerivedT >
class Properties
    :   public PluginVisitor< DerivedT >
{
public:
    const UUID* own_id;

    Properties( const UUID* uuid = new UUID(UUID::generate()) )
        :   PluginVisitor< DerivedT >()
        ,   own_id( uuid )
    {}

    // Please inherit and implement, if you implement own strategies for merging text
    virtual Properties< DerivedT >* visit() = 0;
};



ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_PLUGIN_PROPERTY_H_
