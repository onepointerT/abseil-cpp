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


// A bivariate identity class, that can store one dynamic type like any.
// Unlike any, there is no need to type cast with std::any_cast, and only
// the true type can be adherently obtained.
// NOTE: The compiler will find out about the template parameters of the
//      functions (since C++17).
class VariadicIdentity {

    // The identity pointer to the value
    std::identity* value;
public:
    // The constructor
    template< typename il_t, typename ...Types >
    VariadicIdentity( std::initializer_list<il_t> il = {} )
        :   value( new std::identity<std::variant<Types...>>il )
    {}

    // Set the value.
    template< typename T, typename ...Types >
    void emplace( T val ) {
        std::variant< Types... > vari = this->getValue();
        value = new std::identity<std::variant<T, Types...>>{val};
    }

    // Assign a value.
    template< typename il_t, typename ...Types >
    void operator=( std::initializer_list<il_t> il ) {
        std::variant< Types... > vari = this->getValue();
        value = new std::identity<std::variant<il_t, Types...>>il;
    }

    // Get the type-and-value identity storage.
    std::identity get() const {
        return *this->value;
    }

    // Get the current value as variant.
    template< typename ...Types >
    std::variant<Types...> getValue() const {
        return *this->value;
    }

    // Get the current value as reference.
    template< typename T, typename ...Types >
    T&& operator()() const {
        std::type_identity_t<T> ti_type = this->gettype();
        return *std::get_if<ti_type>(this->getValue<Types...>());
    }

    // Get the type of the currently owned value.
    // Unlike variant or any, it is and will be uniquely be of one type.
    template< typename T >
    std::type_identity_t<T> gettype() const {
        return std::type_identity_t{this->value};
    }
};


ABSL_NAMESPACE_END
} // namespace absl


#endif  // ABSL_BASE_VARIADIC_IDENTITY_H_
