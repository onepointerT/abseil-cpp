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
// This header file contains 
//
// The following abstractions are defined:
//
//   * 
//   * 
//   * 
//

#ifndef ABSL_BASE_CAST_H_
#define ABSL_BASE_CAST_H_

#include <type_traits>

#include "absl/base/casts.h"
#include "absl/base/config.h"

namespace absl {
ABSL_NAMESPACE_BEGIN




template< typename B, typename D >
struct cast
{
    typedef typename std::conditional< std::is_base_of_v< B, D >, D&, B& >::type type;

    static bool is_derived_castable( const B& cls ) {
        return std::is_base_of_v< B, D > 
          && std::is_class_v< D >
          && std::is_same_v< D, decltype(cls) >
          && std::is_pointer_interconvertible_with_class< B, D >( *cls );
    }

    typename type operator()( B& cls ) {
        return (type)cls;
        /*if ( is_derived_castable(cls) ) {
            return absl::implicit_cast<D>(cls); // dynamic_cast< type >( cls );
        }
        return cls;*/
        /*std::enable_if< is_derived_castable( cls ), return (D&)cls; >();
        std::enable_if<!is_derived_castable(cls), return cls; >();*/
    }
};



ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_BASE_CAST_H_

