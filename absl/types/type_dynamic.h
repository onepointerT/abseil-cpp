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
// type_dynamic.h
// -----------------------------------------------------------------------------
//
#ifndef ABSL_TYPES_TYPE_DYNAMIC_H_
#define ABSL_TYPES_TYPE_DYNAMIC_H_

#include <variant>

#include "absl/base/config.h"
#include "absl/types/any.h"


namespace absl {
ABSL_NAMESPACE_BEGIN


// A dynamic typename. Knows of any type it had before and represents it
// bivariadic as a variant of dynamic, exposable commutable and additive type.
// Is only one pointer of size, all other knowledge is brought at compile-time.
// NOTE: The compiler will find out about the template parameters of the
//      functions (since C++17).
class TypeDynamic
{
protected:
  // The knowledge of the type(s) it already had.
  absl::any* type;

public:
  // The constructor. Specify the argument's types and initialize the values.
  template< typename ...Args >
  TypeDynamic( std::initializer_list<Args...> il = {} )
    : type( new absl::any() )
  {
    std::variant<Args...>* val = new std::variant< Args... >();
    types->emplace( val );
  }

  // Get all types, that this instance already has had.
  template< typename ...Args >
  std::variant< Args... >* gettype() const {
    return std::any_cast< std::variant<Args...>* >(*this->type);
  }

  // Emplace a new type inside this instance's type knowledge.
  template< typename T, typename ...Args >
  void emplace( T&& value ) {
    std::variant<Args...>* vari = this->gettype();
    std::variant< T, Args... > vari_new = new std::variant< T, Args... >();
    this->type->emplace( vari_new );
  }
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_TYPES_TYPE_DYNAMIC_H_
