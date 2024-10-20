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
// This header file defines 
//
// 
//



#ifndef ABSL_BASE_VARIADIC_IDENTITY_H_
#define ABSL_BASE_VARIADIC_IDENTITY_H_

#include <functional>
#include <type_traits>
#include <variant>

#include "absl/base/config.h"


namespace absl {
ABSL_NAMESPACE_BEGIN


class VariadicIdentity {

    std::identity value;
public:
    template< typename il_t, typename ...Types >
    VariadicIdentity( std::initializer_list<il_t> il = {} )
        :   value( new std::identity<std::variant<Types...>>il )
    {}

    template< typename T, typename ...Types >
    void emplace( T val ) {
        value = new std::identity<std::variant<Types...>>{val};
    }

    template< typename il_t, typename ...Types >
    void operator=( std::initializer_list<il_t> il ) {
        value = new std::identity<std::variant<Types...>>il;
    }

    std::identity get() const {
        return std::identity(this->value);
    }

    template< typename ...Types >
    std::variant<Types...> getValue() const {
        return this->value;
    }

    template< typename T >
    std::type_identity_t<T> gettype() const {
        return std::type_identity_t{this->value};
    }
};


ABSL_NAMESPACE_END
} // namespace absl


#endif  // ABSL_BASE_VARIADIC_IDENTITY_H_
