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
// File: function_ref_generic.h
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
//  An `absl::FunctionRefGeneric` is similar in usage to a `absl::FunctionRef` but has the
//  following differences:
//
//  * It doesn't have to know the function's arguments before compile time.
//  * It doesn't have a per-parameter-combinational type
//  * It is usable with `absl::FunctionArgumentsAny` and `std::tuple<...>`
//
// Generally, `absl::FunctionRefGeneric` should not be used as a return value, data
// member, or to initialize a `std::function`. Such usages will often lead to
// problematic lifetime issues. Once you convert something to an
// `absl::FunctionRefGeneric` you cannot make a deep copy later.
//
// This class is suitable for use wherever a "const std::function<>&"
// would be used without making a copy. ForEach functions and other versions of
// the visitor pattern are a good example of when this class should be used.
//
// This class is trivial to copy and should be passed by value.
#ifndef ABSL_FUNCTIONAL_FUNCTION_REF_GENERIC_H_
#define ABSL_FUNCTIONAL_FUNCTION_REF_GENERIC_H_

#include <cassert>
#include <functional>
#include <type_traits>

#include "absl/functional/function_arguments_any.h"
#include "absl/functional/function_ref.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


// FunctionRef
//
// An `absl::FunctionRefGeneric` is a lightweight wrapper to any invocable object with
// a compatible signature. Generally, an `absl::FunctionRefGeneric` should only be used
// as an argument type and should be preferred as an argument over a const
// reference to a `std::function`. `absl::FunctionRefGeneric` itself does not allocate,
// although the wrapped invocable may.
//
// Example:
//
//   // The following function takes a function callback by const reference
//   bool Visitor(const std::function<void(my_proto&,
//                                         absl::string_view)>& callback);
//
//   // Assuming that the function is not stored or otherwise copied, it can be
//   // replaced by an `absl::FunctionRef`:
//   bool Visitor(absl::FunctionRef<void(my_proto&, absl::string_view)>
//                  callback);
//
// Note: the assignment operator within an `absl::FunctionRefGeneric` is intentionally
// deleted to prevent misuse; because the `absl::FunctionRefGeneric` does not own the
// underlying type, assignment likely indicates misuse.
template <typename R>
class FunctionRefGeneric
{
public:
  template< typename ...Args >
  using func_t = R (*)(Args...);
  template< typename ...Args >
  using funcref_t = FunctionRef<R(Args...)>;

private:
  std::any* func_ref_arguments;

  // Used to disable constructors for objects that are not compatible with the
  // signature of this FunctionRef.
  template <typename F,
            typename FR = absl::base_internal::invoke_result_t<F, Args&&...>>
  using EnableIfCompatible =
      typename std::enable_if<std::is_void<R>::value ||
                              std::is_convertible<FR, R>::value>::type;

 public:
  // Constructs a FunctionRefGeneric from any invocable type.
  template <typename F, typename ...Args >
  FunctionRefGeneric( R (*f)(FunctionArgumentsAny*) ABSL_ATTRIBUTE_LIFETIME_BOUND)
      : invoker_(&absl::functional_internal::InvokeObject<F, R, Args...>) {
    absl::functional_internal::AssertNonNull(f);
    ptr_.obj = &f;
  }
  // Constructs a FunctionRefGeneric from any invocable type.
  template <typename F, typename ...Args >
  FunctionRefGeneric( R (*f)(FunctionArgumentsAny) ABSL_ATTRIBUTE_LIFETIME_BOUND)
      : invoker_(&absl::functional_internal::InvokeObject<F, R, Args...>) {
    absl::functional_internal::AssertNonNull(f);
    ptr_.obj = &f;
  }
  // Constructs a FunctionRefGeneric from any invocable type.
  template <typename F, typename ...Args >
  FunctionRefGeneric( R (*f)(Args...) ABSL_ATTRIBUTE_LIFETIME_BOUND)
      : invoker_(&absl::functional_internal::InvokeObject<F, R, Args...>) {
    absl::functional_internal::AssertNonNull(f);
    ptr_.obj = &f;
  }

  // Overload for function pointers. This eliminates a level of indirection that
  // would happen if the above overload was used (it lets us store the pointer
  // instead of a pointer to a pointer).
  //
  // This overload is also used for references to functions, since references to
  // functions can decay to function pointers implicitly.
  template <
      typename F, typename = EnableIfCompatible<F*>,
      absl::functional_internal::EnableIf<absl::is_function<F>::value> = 0>
  FunctionRef(F* f)  // NOLINT(runtime/explicit)
      : invoker_(&absl::functional_internal::InvokeFunction<F*, R, Args...>) {
    assert(f != nullptr);
    ptr_.fun = reinterpret_cast<decltype(ptr_.fun)>(f);
  }

  // To help prevent subtle lifetime bugs, FunctionRef is not assignable.
  // Typically, it should only be used as an argument type.
  FunctionRefGeneric& operator=(const FunctionRefGeneric& rhs) = delete;
  FunctionRefGeneric(const FunctionRefGeneric& rhs) = default;

  // Call the underlying object.
  template< typename ...Args >
  R operator()(Args... args) const {
    return invoker_(ptr_, std::forward<Args>(args)...);
  }
  // Call the underlying object.
  template< typename ...Args >
  R operator()(std::tuple<Args...> args) const {
    return invoker_(ptr_, std::forward<Args...>(args));
  }
  // Call the underlying object.
  template< typename ...Args >
  R operator()(FunctionArgumentsAny args) const {
    return operator()(args.getall_tuple());
  }
  // Call the underlying object.
  template< typename ...Args >
  R operator()(FunctionArgumentsAny* args) const {
    return operator()(args->getall_tuple());
  }

 private:
  absl::functional_internal::VoidPtr ptr_;
  absl::functional_internal::Invoker<R, Args...> invoker_;
};

ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_FUNCTIONAL_FUNCTION_REF_GENERIC_H_
