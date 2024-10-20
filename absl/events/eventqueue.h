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

#ifndef ABSL_EVENTS_EVENTQUEUE_H_
#define ABSL_EVENTS_EVENTQUEUE_H_

#include <initializer_list>
#include <string>
#include <utility>
#include <variant>

#include "absl/base/config.h"
#include "absl/functional/function_arguments.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


template< typename base_cls_t >
class EventObject;


template< typename ...il_t >
using event_element = std::variant< il_t... >;

template< typename ...il_t >
using event_queue = std::tuple< std::initializer_list<event_element<il_t...>> >;


template< typename base_cls_t, typename ...il_t >
class EventQueue
    :   protected event_queue<il_t...>
{
protected:
    friend class EventObject<base_cls_t>;
};

ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_EVENTS_EVENTQUEUE_H_
