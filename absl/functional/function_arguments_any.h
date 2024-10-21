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
// File: function_arguments_any.h
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
#ifndef ABSL_FUNCTIONAL_FUNCTION_ARGUMENTS_ANY_H_
#define ABSL_FUNCTIONAL_FUNCTION_ARGUMENTS_ANY_H_



#include "absl/base/config.h"
#include "absl/functional/function_arguments.h"
#include "absl/types/any.h"


namespace absl {
ABSL_NAMESPACE_BEGIN


// A specialization for `absl::FunctionArguments` where the
// template parameter is first expected to be known on construction time
// of the object and can be pushed into arrays without having one
// list per template parameter difference.
// NOTE: You can use the functions without specifying the moreful unpredictable
// template parameters, which will be substituted by the compiler at at least C++17.
class FunctionArgumentsAny
  : protected absl::any
{
protected:
  // The pointer to the specific `absl::FunctionArguments<...>` object
  absl::any* fargs;

public:
  // The constructor. Specify the argument's types and initialize the values.
  template< typename ...Args >
  FunctionArgumentsAny( Args... args, std::initializer_list<Args...> il )
    : fargs( new absl::any() )
  {
    FunctionArguments<Args...>* farg = new FunctionArguments<Args...>(args, il);
    fargs->emplace( fargs );
  }

  // Get the specific `absl::FunctionArguments<...>` object for further
  // speciality, using and functionality
  template< typename T, typename ...Args >
  FunctionArguments<Args...>* args() {
    return absl::any_cast<T>( *fargs );
  }

  // Get to know all types of the function arguments.
  // Also have a look at https://en.cppreference.com/w/cpp/utility/variant/holds_alternative
  template< typename ...Args >
  std::variant< Args... > types() const {
    FunctionArguments<Args...>* fa = this->args();
    return std::variant<Args...>();
  }

  // Get all values as a tuple.
  template< typename ...Args >
  std::tuple< Args... > getall_tuple() const {
    return this->args()->getall_tuple();
  }
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_FUNCTIONAL_FUNCTION_ARGUMENTS_ANY_H_
