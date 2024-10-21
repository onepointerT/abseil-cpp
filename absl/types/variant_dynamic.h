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
// variant_dynamic.h
// -----------------------------------------------------------------------------
//
#ifndef ABSL_TYPES_VARIANT_DYNAMIC_H_
#define ABSL_TYPES_VARIANT_DYNAMIC_H_



#include "absl/base/config.h"
#include "absl/base/variadic_identity.h"
#include "absl/types/type_dynamic.h"


namespace absl {
ABSL_NAMESPACE_BEGIN


// A Variant, that dynamically knows all types, it already has had and also
// dynamically stores all value
class VariantDynamic final
    :   public absl::VariadicIdentity
    ,   public absl::TypeDynamic
{
protected:

public:
  // The constructor. Specify the argument's types and initialize the values.
  template< typename ...Args >
  VariantDynamic( std::initializer_list<Args...> il )
    : VariadicIdentity( il )
    , types( new absl::any() )
  {
    std::variant<Args...> val = new std::variant< Args... >();
    this->emplace( val );
  }

  // Emplace a new value to the instance and also acknowledge the new type.
  template< typename T, typename ...Args >
  void emplace( T&& value ) {
    VariadicIdentity::emplace( value );
    TypeDynamic::emplace(value);
  }

  // Emplace a new value to the instance and also acknowledge the new type,
  // if it is a new type.
  template< typename T >
  void operator=( T&& value ) {
    this->emplace( value );
  }
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_TYPES_VARIANT_DYNAMIC_H_
