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
// This header file defines the `absl::FunctionRef` type for holding a
// non-owning reference to an object of any invocable type. This function
// reference is typically most useful as a type-erased argument type for
// accepting function types that neither take ownership nor copy the type; using
// the reference type in this case avoids a copy and an allocation. Best
// practices of other non-owning reference-like objects (such as
// `absl::string_view`) apply here.
//
//  An `absl::FunctionRef` is similar in usage to a `std::function` but has the
//  following differences:
//
//  * It doesn't own the underlying object.
//  * It doesn't have a null or empty state.
//  * It never performs deep copies or allocations.
//  * It's much faster and cheaper to construct.
//  * It's trivially copyable and destructable.
//
// Generally, `absl::FunctionRef` should not be used as a return value, data
// member, or to initialize a `std::function`. Such usages will often lead to
// problematic lifetime issues. Once you convert something to an
// `absl::FunctionRef` you cannot make a deep copy later.
//
// This class is suitable for use wherever a "const std::function<>&"
// would be used without making a copy. ForEach functions and other versions of
// the visitor pattern are a good example of when this class should be used.
//
// This class is trivial to copy and should be passed by value.
#ifndef ABSL_FUNCTIONAL_FUNCTION_ARGUMENTS_H_
#define ABSL_FUNCTIONAL_FUNCTION_ARGUMENTS_H_

#include <functional>
#include <tuple>
#include <type_traits>

#include "absl/base/config.h"


namespace absl {
ABSL_NAMESPACE_BEGIN


template< typename ...Args >
class FunctionArguments
{
public:
  std::tuple< Args... > types;
  std::vector< struct std::identity > values;

//  typedef std::ignore ignore;

  FunctionArguments( Args... args, std::initializer_list<Args...> il )
    : types( std::make_tuple<Args...>() )
    , values( il )
  {}

  template< std::size_t I >
  auto type( const unsigned int argc = 0 ) {
    if ( argc >= std::tuple_size_v<std::tuple<Args...>>() ) {
      return std::false_type();
    }
    return std::get<argc>(this->types);
  }

  template< std::size_t I, typename ...a >
  std::tuple_element_t< I, std::tuple<a...> > alltypes() {
    return std::tuple_element_t<sizeof...(Args...), this->types>;
  }

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

  auto get( const unsigned int argc ) {
    if ( argc >= std::tuple_size_v<std::tuple<Args...>>() ) {
      return std::false_type();
    }

    return this->values.at(argc)();
  }
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_FUNCTIONAL_FUNCTION_ARGUMENTS_H_
