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
// -----------------------------------------------------------------------------
// File: function_arguments.h
// -----------------------------------------------------------------------------
//
// This header file defines the `absl::FunctionArgumentsAny` type for holding a
// non-owning reference to an object of any unpredictable type pack (nor variant).
// This function reference is typically most useful as a type-erased argument type for
// accepting in function types that neither take ownership nor copy the type; using
// the reference type in this case avoids a copy and an allocation.
//
//  An `absl::FunctionArgumentsAny` is similar in usage to a `absl::FunctionArguments`
//  but has the following differences:
//
//  * It doesn't define a type before use
//  * It does have an any state
//  * It never performs deep copies or allocations.
//  * It's much faster and cheaper to construct.
//  * It's trivially copyable and destructable.
//
// Generally, `absl::FunctionArgumentsAny` should be used as a variable value, data
// member can be accessed with `absl::FunctionArgumentsAny::args()` which then congests
// at compile time which types the `absl::FunctionArguments` hold and does not need to
// know them, similar like any, at store-and-definition time.
//
// This class is suitable for use wherever a "const std::function<>&"
// would be used without making a copy or substitutes a set of parameters (e.g. 
// `absl::FunctionRefGeneric::operator()` does this).
//
// This class is trivial to copy and should be passed by value or pointer.
#ifndef ABSL_FUNCTIONAL_FUNCTION_ARGUMENTS_H_
#define ABSL_FUNCTIONAL_FUNCTION_ARGUMENTS_H_

#include <functional>
#include <tuple>
#include <type_traits>

#include "absl/base/config.h"
#include "absl/base/variadic_identity.h"


namespace absl {
ABSL_NAMESPACE_BEGIN


// A class that holds the types of arguments and its values
template< typename ...Args >
class FunctionArguments
{
public:
  // The types of all function arguments
  std::tuple< Args... > types;
  // The values of all functional arguments
  // The index `i` of types is adherent to `values[i]` true value's type
  std::vector< struct std::identity > values;

  // The constructor. Specify the argument's types and initialize the values.
  FunctionArguments( Args... args, std::initializer_list<Args...> il )
    : types( std::make_tuple<Args...>() )
    , values( il )
  {}
  // The constructor. Specify and initialize the values.
  FunctionArguments( std::initializer_list<Args...>& il )
    : types( std::make_tuple<Args...>() )
    , values( std::enable_if<sizeof...(Args...) > 0, il>() )
  {}

  // Get the type at position argc.
  template< std::size_t I, typename ...a >
  std::tuple_element_t< I, std::tuple<a...> > type( const unsigned int argc = 0 ) {
    if ( argc >= std::tuple_size_v<std::tuple<Args...>>() ) {
      return std::false_type();
    }
    return std::get<argc>(this->types);
  }

  // Get all types as `tuple_element_t`-like list
  template< std::size_t I, typename ...a >
  std::tuple_element_t< I, std::tuple<a...> > alltypes() {
    return std::tuple_element_t<sizeof...(Args...), this->types>;
  }

  // Get a variant for further inspection on the types of the parameter pack.
  // Also have a look at https://en.cppreference.com/w/cpp/utility/variant/holds_alternative
  std::variant< Args... > alltypes_v() {
    return std::variant< Args... >();
  }

  // Set the positional value at a position of the parameter or type pack.
  template< typename Argv >
  bool set_value( const unsigned int argc, Argv&& argv ) {
    if ( argc >= std::tuple_size_v<this->types>() ) {
      return false;
    } else if ( this->type(argc) == decltype(Argv) ) {
      this->values[argc] = std::identity<Argv&&>(argv);
      return true;
    }
    return false;
  }

  // Get the value at an arguments's position.
  auto get( const unsigned int argc ) {
    if ( argc >= std::tuple_size_v<std::tuple<Args...>>() ) {
      return std::false_type();
    }

    return this->values.at(argc)();
  }

  // Get all values as initializer list.
  // The true value can be accessed from the list with
  //    `std::variant<Types...> getValue() const`
  std::initializer_list<VariadicIdentity>& getall() const {
    std::initializer_list<VariadicIdentity> il;
    std::copy( this->values.begin(), this->values.back(), il );
    return il;
  }

  // Get all values as a tuple
  template< typename ...Args >
  std::tuple< Args... > getall_tuple() const {
    auto il = this->getall();
    decltype(this->values) tpl;
    std::copy( il.begin(), il.end(), tpl );
    return tpl;
  }
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_FUNCTIONAL_FUNCTION_ARGUMENTS_H_
