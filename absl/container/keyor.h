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
// File: keyor.h
// -----------------------------------------------------------------------------
//
// An `absl::KeyOr<T>` represents a union of an `absl::Status` object
// and an object of type `T`. The `absl::KeyOr<T>` will either contain an
// object of type `T` (indicating a successful operation), or an key (of type
// `absl::Key`) explaining why such a value is not present.
//
// In general, check the success of an operation returning an
// `absl::KeyOr<T>` like you would an `absl::Status` by using the `ok()`
// member function.
//
// Example:
//
//   KeyOr<Foo> result = Calculation();
//   if (result.ok()) {
//     result->DoSomethingCool();
//   } else {
//     LOG(ERROR) << result.status();
//   }
#ifndef ABSL_STATUS_KeyOr_H_
#define ABSL_STATUS_KeyOr_H_

#include <span>
#include <type_traits>

#include "absl/base/bivariate_return_value.h"
#include "absl/container/key.h"
#include "absl/container/internal/keyor_internal.h"
#include "absl/strings/string_view.h"

namespace absl {
ABSL_NAMESPACE_BEGIN

// BadKeyOrAccess
//
// This class defines the type of object to throw (if exceptions are enabled),
// when accessing the value of an `absl::KeyOr<T>` object that does not
// contain a value. This behavior is analogous to that of
// `std::bad_optional_access` in the case of accessing an invalid
// `std::optional` value.
//
// Example:
//
// try {
//   absl::KeyOr<int> v = FetchInt();
//   DoWork(v.value());  // Accessing value() when not "OK" may throw
// } catch (absl::BadKeyOrAccess& ex) {
//   LOG(ERROR) << ex.status();
// }
class BadKeyOrAccess : public absl::BadStatusOrAccess {};
class BadValueOrAccess : public absl::BadKeyOrAccess {};


// Returned KeyOr objects may not be ignored.
template <typename KeyT, typename ValueT>
#if ABSL_HAVE_CPP_ATTRIBUTE(nodiscard)
// TODO(b/176172494): ABSL_MUST_USE_RESULT should expand to the more strict
// [[nodiscard]]. For now, just use [[nodiscard]] directly when it is available.
class [[nodiscard]] KeyOr;
#else
class ABSL_MUST_USE_RESULT KeyOr;
#endif  // ABSL_HAVE_CPP_ATTRIBUTE(nodiscard)

// absl::KeyOr<T>
//
// The `absl::KeyOr<T>` class template is a union of an `absl::Status` object
// and an object of type `T`. The `absl::KeyOr<T>` models an object that is
// either a usable object, or an error (of type `absl::Status`) explaining why
// such an object is not present. An `absl::KeyOr<T>` is typically the return
// value of a function which may fail.
//
// An `absl::KeyOr<T>` can never hold an "OK" status (an
// `absl::StatusCode::kOk` value); instead, the presence of an object of type
// `T` indicates success. Instead of checking for a `kOk` value, use the
// `absl::KeyOr<T>::ok()` member function. (It is for this reason, and code
// readability, that using the `ok()` function is preferred for `absl::Status`
// as well.)
//
// Example:
//
//   KeyOr<Foo> result = DoBigCalculationThatCouldFail();
//   if (result.ok()) {
//     result->DoSomethingCool();
//   } else {
//     LOG(ERROR) << result.status();
//   }
//
// Accessing the object held by an `absl::KeyOr<T>` should be performed via
// `operator*` or `operator->`, after a call to `ok()` confirms that the
// `absl::KeyOr<T>` holds an object of type `T`:
//
// Example:
//
//   absl::KeyOr<int> i = GetCount();
//   if (i.ok()) {
//     updated_total += *i;
//   }
//
// NOTE: using `absl::KeyOr<T>::value()` when no valid value is present will
// throw an exception if exceptions are enabled or terminate the process when
// exceptions are not enabled.
//
// Example:
//
//   KeyOr<Foo> result = DoBigCalculationThatCouldFail();
//   const Foo& foo = result.value();    // Crash/exception if no value present
//   foo.DoSomethingCool();
//
// A `absl::KeyOr<T*>` can be constructed from a null pointer like any other
// pointer value, and the result will be that `ok()` returns `true` and
// `value()` returns `nullptr`. Checking the value of pointer in an
// `absl::KeyOr<T*>` generally requires a bit more care, to ensure both that
// a value is present and that value is not null:
//
//  KeyOr<std::unique_ptr<Foo>> result = FooFactory::MakeNewFoo(arg);
//  if (!result.ok()) {
//    LOG(ERROR) << result.status();
//  } else if (*result == nullptr) {
//    LOG(ERROR) << "Unexpected null pointer";
//  } else {
//    (*result)->DoSomethingCool();
//  }
//
// Example factory implementation returning KeyOr<T>:
//
//  KeyOr<Foo> FooFactory::MakeFoo(int arg) {
//    if (arg <= 0) {
//      return absl::Status(absl::StatusCode::kInvalidArgument,
//                          "Arg must be positive");
//    }
//    return Foo(arg);
//  }
template <typename KeyT, typename ValueT>
class KeyOr
  : public KeyValuePair< KeyT, ValueT >
  , protected std::span< ValueT >
{
  template <typename U>
  friend class KeyOr;

  typedef internal_KeyOr::KeyOrData<T> Base;

 public:
  // KeyOr<T>::value_type
  //
  // This instance data provides a generic `value_type` member for use within
  // generic programming. This usage is analogous to that of
  // `optional::value_type` in the case of `std::optional`.
  typedef ValueT value_type;

  // On bivariate returns a false indicates the second type and a true on the
  // inspection function indicates the second type.
  static bool bivariate_return( KeyT key, value_type value ) {
    if ( ! value->empty() )
      return false;
    return true;
  }

  bool has_value() const { return !this->empty(); }

  typedef typename BivariateReturn< Key< KeyT >, ValueT*, &KeyOr< KeyT, ValueT >::bivariate_return, this, this > type;

  type get() {
    if ( this->has_value() ) {
      return this->data();
    }
    return Key< KeyT >;
  }

  void set( value_type value ) {
    std::span< ValueT >::operator=( std::span< ValueT >(value) );
  }

  // Constructors

  // Constructs a new `absl::KeyOr` with an `absl::StatusCode::kUnknown`
  // status. This constructor is marked 'explicit' to prevent usages in return
  // values such as 'return {};', under the misconception that
  // `absl::KeyOr<std::vector<int>>` will be initialized with an empty
  // vector, instead of an `absl::StatusCode::kUnknown` error code.
  explicit KeyOr();

  // `KeyOr<T>` is copy constructible if `T` is copy constructible.
  KeyOr(const KeyOr&) = default;
  // `KeyOr<T>` is copy assignable if `T` is copy constructible and copy
  // assignable.
  KeyOr& operator=(const KeyOr&) = default;

  // `KeyOr<T>` is move constructible if `T` is move constructible.
  KeyOr(KeyOr&&) = default;
  // `KeyOr<T>` is moveAssignable if `T` is move constructible and move
  // assignable.
  KeyOr& operator=(KeyOr&&) = default;

  // Converting Constructors

  // Constructs a new `absl::KeyOr<T>` from an `absl::KeyOr<U>`, when `T`
  // is constructible from `U`. To avoid ambiguity, these constructors are
  // disabled if `T` is also constructible from `KeyOr<U>.`. This constructor
  // is explicit if and only if the corresponding construction of `T` from `U`
  // is explicit. (This constructor inherits its explicitness from the
  // underlying constructor.)
  template <typename U, absl::enable_if_t<
                            internal_KeyOr::IsConstructionFromKeyOrValid<
                                false, T, U, false, const U&>::value,
                            int> = 0>
  KeyOr(const KeyOr<U>& other)  // NOLINT
      : Base(static_cast<const typename KeyOr<U>::Base&>(other)) {}
  template <typename U, absl::enable_if_t<
                            internal_KeyOr::IsConstructionFromKeyOrValid<
                                false, T, U, true, const U&>::value,
                            int> = 0>
  KeyOr(const KeyOr<U>& other ABSL_ATTRIBUTE_LIFETIME_BOUND)  // NOLINT
      : Base(static_cast<const typename KeyOr<U>::Base&>(other)) {}
  template <typename U, absl::enable_if_t<
                            internal_KeyOr::IsConstructionFromKeyOrValid<
                                true, T, U, false, const U&>::value,
                            int> = 0>
  explicit KeyOr(const KeyOr<U>& other)
      : Base(static_cast<const typename KeyOr<U>::Base&>(other)) {}
  template <typename U, absl::enable_if_t<
                            internal_KeyOr::IsConstructionFromKeyOrValid<
                                true, T, U, true, const U&>::value,
                            int> = 0>
  explicit KeyOr(const KeyOr<U>& other ABSL_ATTRIBUTE_LIFETIME_BOUND)
      : Base(static_cast<const typename KeyOr<U>::Base&>(other)) {}

  template <typename U, absl::enable_if_t<
                            internal_KeyOr::IsConstructionFromKeyOrValid<
                                false, T, U, false, U&&>::value,
                            int> = 0>
  KeyOr(KeyOr<U>&& other)  // NOLINT
      : Base(static_cast<typename KeyOr<U>::Base&&>(other)) {}
  template <typename U, absl::enable_if_t<
                            internal_KeyOr::IsConstructionFromKeyOrValid<
                                false, T, U, true, U&&>::value,
                            int> = 0>
  KeyOr(KeyOr<U>&& other ABSL_ATTRIBUTE_LIFETIME_BOUND)  // NOLINT
      : Base(static_cast<typename KeyOr<U>::Base&&>(other)) {}
  template <typename U, absl::enable_if_t<
                            internal_KeyOr::IsConstructionFromKeyOrValid<
                                true, T, U, false, U&&>::value,
                            int> = 0>
  explicit KeyOr(KeyOr<U>&& other)
      : Base(static_cast<typename KeyOr<U>::Base&&>(other)) {}
  template <typename U, absl::enable_if_t<
                            internal_KeyOr::IsConstructionFromKeyOrValid<
                                true, T, U, true, U&&>::value,
                            int> = 0>
  explicit KeyOr(KeyOr<U>&& other ABSL_ATTRIBUTE_LIFETIME_BOUND)
      : Base(static_cast<typename KeyOr<U>::Base&&>(other)) {}

  // Access operators
  

  // Converting Assignment Operators

  // Creates an `absl::KeyOr<T>` through assignment from an
  // `absl::KeyOr<U>` when:
  //
  //   * Both `absl::KeyOr<T>` and `absl::KeyOr<U>` are OK by assigning
  //     `U` to `T` directly.
  //   * `absl::KeyOr<T>` is OK and `absl::KeyOr<U>` contains an error
  //      code by destroying `absl::KeyOr<T>`'s value and assigning from
  //      `absl::KeyOr<U>'
  //   * `absl::KeyOr<T>` contains an error code and `absl::KeyOr<U>` is
  //      OK by directly initializing `T` from `U`.
  //   * Both `absl::KeyOr<T>` and `absl::KeyOr<U>` contain an error
  //     code by assigning the `Status` in `absl::KeyOr<U>` to
  //     `absl::KeyOr<T>`
  //
  // These overloads only apply if `absl::KeyOr<T>` is constructible and
  // assignable from `absl::KeyOr<U>` and `KeyOr<T>` cannot be directly
  // assigned from `KeyOr<U>`.
  template <typename U,
            absl::enable_if_t<internal_KeyOr::IsKeyOrAssignmentValid<
                                  T, const U&, false>::value,
                              int> = 0>
  KeyOr& operator=(const KeyOr<U>& other) {
    this->Assign(other);
    return *this;
  }
  template <typename U,
            absl::enable_if_t<internal_KeyOr::IsKeyOrAssignmentValid<
                                  T, const U&, true>::value,
                              int> = 0>
  KeyOr& operator=(const KeyOr<U>& other ABSL_ATTRIBUTE_LIFETIME_BOUND) {
    this->Assign(other);
    return *this;
  }
  template <typename U,
            absl::enable_if_t<internal_KeyOr::IsKeyOrAssignmentValid<
                                  T, U&&, false>::value,
                              int> = 0>
  KeyOr& operator=(KeyOr<U>&& other) {
    this->Assign(std::move(other));
    return *this;
  }
  template <typename U,
            absl::enable_if_t<internal_KeyOr::IsKeyOrAssignmentValid<
                                  T, U&&, true>::value,
                              int> = 0>
  KeyOr& operator=(KeyOr<U>&& other ABSL_ATTRIBUTE_LIFETIME_BOUND) {
    this->Assign(std::move(other));
    return *this;
  }

  // Constructs a new `absl::KeyOr<T>` with a non-ok status. After calling
  // this constructor, `this->ok()` will be `false` and calls to `value()` will
  // crash, or produce an exception if exceptions are enabled.
  //
  // The constructor also takes any type `U` that is convertible to
  // `absl::Status`. This constructor is explicit if an only if `U` is not of
  // type `absl::Status` and the conversion from `U` to `Status` is explicit.
  //
  // REQUIRES: !Status(std::forward<U>(v)).ok(). This requirement is DCHECKed.
  // In optimized builds, passing absl::OkStatus() here will have the effect
  // of passing absl::StatusCode::kInternal as a fallback.
  template <typename U = absl::Status,
            absl::enable_if_t<internal_KeyOr::IsConstructionFromStatusValid<
                                  false, T, U>::value,
                              int> = 0>
  KeyOr(U&& v) : Base(std::forward<U>(v)) {}

  template <typename U = absl::Status,
            absl::enable_if_t<internal_KeyOr::IsConstructionFromStatusValid<
                                  true, T, U>::value,
                              int> = 0>
  explicit KeyOr(U&& v) : Base(std::forward<U>(v)) {}
  template <typename U = absl::Status,
            absl::enable_if_t<internal_KeyOr::IsConstructionFromStatusValid<
                                  false, T, U>::value,
                              int> = 0>
  KeyOr& operator=(U&& v) {
    this->AssignStatus(std::forward<U>(v));
    return *this;
  }

  // Perfect-forwarding value assignment operator.

  // If `*this` contains a `T` value before the call, the contained value is
  // assigned from `std::forward<U>(v)`; Otherwise, it is directly-initialized
  // from `std::forward<U>(v)`.
  // This function does not participate in overload unless:
  // 1. `std::is_constructible_v<T, U>` is true,
  // 2. `std::is_assignable_v<T&, U>` is true.
  // 3. `std::is_same_v<KeyOr<T>, std::remove_cvref_t<U>>` is false.
  // 4. Assigning `U` to `T` is not ambiguous:
  //  If `U` is `KeyOr<V>` and `T` is constructible and assignable from
  //  both `KeyOr<V>` and `V`, the assignment is considered bug-prone and
  //  ambiguous thus will fail to compile. For example:
  //    KeyOr<bool> s1 = true;  // s1.ok() && *s1 == true
  //    KeyOr<bool> s2 = false;  // s2.ok() && *s2 == false
  //    s1 = s2;  // ambiguous, `s1 = *s2` or `s1 = bool(s2)`?
  template <typename U = T,
            typename std::enable_if<
                internal_KeyOr::IsAssignmentValid<T, U, false>::value,
                int>::type = 0>
  KeyOr& operator=(U&& v) {
    this->Assign(std::forward<U>(v));
    return *this;
  }
  template <typename U = T,
            typename std::enable_if<
                internal_KeyOr::IsAssignmentValid<T, U, true>::value,
                int>::type = 0>
  KeyOr& operator=(U&& v ABSL_ATTRIBUTE_LIFETIME_BOUND) {
    this->Assign(std::forward<U>(v));
    return *this;
  }

  // Constructs the inner value `T` in-place using the provided args, using the
  // `T(args...)` constructor.
  template <typename... Args>
  explicit KeyOr(absl::in_place_t, Args&&... args);
  template <typename U, typename... Args>
  explicit KeyOr(absl::in_place_t, std::initializer_list<U> ilist,
                    Args&&... args);

  // Constructs the inner value `T` in-place using the provided args, using the
  // `T(U)` (direct-initialization) constructor. This constructor is only valid
  // if `T` can be constructed from a `U`. Can accept move or copy constructors.
  //
  // This constructor is explicit if `U` is not convertible to `T`. To avoid
  // ambiguity, this constructor is disabled if `U` is a `KeyOr<J>`, where
  // `J` is convertible to `T`.
  template <typename U = T,
            absl::enable_if_t<internal_KeyOr::IsConstructionValid<
                                  false, T, U, false>::value,
                              int> = 0>
  KeyOr(U&& u)  // NOLINT
      : KeyOr(absl::in_place, std::forward<U>(u)) {}
  template <typename U = T,
            absl::enable_if_t<internal_KeyOr::IsConstructionValid<
                                  false, T, U, true>::value,
                              int> = 0>
  KeyOr(U&& u ABSL_ATTRIBUTE_LIFETIME_BOUND)  // NOLINT
      : KeyOr(absl::in_place, std::forward<U>(u)) {}

  template <typename U = T,
            absl::enable_if_t<internal_KeyOr::IsConstructionValid<
                                  true, T, U, false>::value,
                              int> = 0>
  explicit KeyOr(U&& u)  // NOLINT
      : KeyOr(absl::in_place, std::forward<U>(u)) {}
  template <typename U = T,
            absl::enable_if_t<
                internal_KeyOr::IsConstructionValid<true, T, U, true>::value,
                int> = 0>
  explicit KeyOr(U&& u ABSL_ATTRIBUTE_LIFETIME_BOUND)  // NOLINT
      : KeyOr(absl::in_place, std::forward<U>(u)) {}

  // KeyOr<T>::ok()
  //
  // Returns whether or not this `absl::KeyOr<T>` holds a `T` value. This
  // member function is analogous to `absl::Status::ok()` and should be used
  // similarly to check the status of return values.
  //
  // Example:
  //
  // KeyOr<Foo> result = DoBigCalculationThatCouldFail();
  // if (result.ok()) {
  //    // Handle result
  // else {
  //    // Handle error
  // }
  ABSL_MUST_USE_RESULT bool ok() const { return this->ok(); }

  // KeyOr<T>::status()
  //
  // Returns a reference to the current `absl::Status` contained within the
  // `absl::KeyOr<T>`. If `absl::KeyOr<T>` contains a `T`, then this
  // function returns `absl::OkStatus()`.
  const Status& status() const&;
  Status status() &&;

  // KeyOr<T>::value()
  //
  // Returns a reference to the held value if `this->ok()`. Otherwise, throws
  // `absl::BadKeyOrAccess` if exceptions are enabled, or is guaranteed to
  // terminate the process if exceptions are disabled.
  //
  // If you have already checked the status using `this->ok()`, you probably
  // want to use `operator*()` or `operator->()` to access the value instead of
  // `value`.
  //
  // Note: for value types that are cheap to copy, prefer simple code:
  //
  //   T value = KeyOr.value();
  //
  // Otherwise, if the value type is expensive to copy, but can be left
  // in the KeyOr, simply assign to a reference:
  //
  //   T& value = KeyOr.value();  // or `const T&`
  //
  // Otherwise, if the value type supports an efficient move, it can be
  // used as follows:
  //
  //   T value = std::move(KeyOr).value();
  //
  // The `std::move` on KeyOr instead of on the whole expression enables
  // warnings about possible uses of the KeyOr object after the move.
  const T& value() const& ABSL_ATTRIBUTE_LIFETIME_BOUND;
  T& value() & ABSL_ATTRIBUTE_LIFETIME_BOUND;
  const T&& value() const&& ABSL_ATTRIBUTE_LIFETIME_BOUND;
  T&& value() && ABSL_ATTRIBUTE_LIFETIME_BOUND;

  // KeyOr<T>:: operator*()
  //
  // Returns a reference to the current value.
  //
  // REQUIRES: `this->ok() == true`, otherwise the behavior is undefined.
  //
  // Use `this->ok()` to verify that there is a current value within the
  // `absl::KeyOr<T>`. Alternatively, see the `value()` member function for a
  // similar API that guarantees crashing or throwing an exception if there is
  // no current value.
  const T& operator*() const& ABSL_ATTRIBUTE_LIFETIME_BOUND;
  T& operator*() & ABSL_ATTRIBUTE_LIFETIME_BOUND;
  const T&& operator*() const&& ABSL_ATTRIBUTE_LIFETIME_BOUND;
  T&& operator*() && ABSL_ATTRIBUTE_LIFETIME_BOUND;

  // KeyOr<T>::operator->()
  //
  // Returns a pointer to the current value.
  //
  // REQUIRES: `this->ok() == true`, otherwise the behavior is undefined.
  //
  // Use `this->ok()` to verify that there is a current value.
  const T* operator->() const ABSL_ATTRIBUTE_LIFETIME_BOUND;
  T* operator->() ABSL_ATTRIBUTE_LIFETIME_BOUND;

  // KeyOr<T>::value_or()
  //
  // Returns the current value if `this->ok() == true`. Otherwise constructs a
  // value using the provided `default_value`.
  //
  // Unlike `value`, this function returns by value, copying the current value
  // if necessary. If the value type supports an efficient move, it can be used
  // as follows:
  //
  //   T value = std::move(KeyOr).value_or(def);
  //
  // Unlike with `value`, calling `std::move()` on the result of `value_or` will
  // still trigger a copy.
  template <typename U>
  T value_or(U&& default_value) const&;
  template <typename U>
  T value_or(U&& default_value) &&;

  // KeyOr<T>::IgnoreError()
  //
  // Ignores any errors. This method does nothing except potentially suppress
  // complaints from any tools that are checking that errors are not dropped on
  // the floor.
  void IgnoreError() const;

  // KeyOr<T>::emplace()
  //
  // Reconstructs the inner value T in-place using the provided args, using the
  // T(args...) constructor. Returns reference to the reconstructed `T`.
  template <typename... Args>
  T& emplace(Args&&... args) ABSL_ATTRIBUTE_LIFETIME_BOUND {
    if (ok()) {
      this->Clear();
      this->MakeValue(std::forward<Args>(args)...);
    } else {
      this->MakeValue(std::forward<Args>(args)...);
      this->status_ = absl::OkStatus();
    }
    return this->data_;
  }

  template <
      typename U, typename... Args,
      absl::enable_if_t<
          std::is_constructible<T, std::initializer_list<U>&, Args&&...>::value,
          int> = 0>
  T& emplace(std::initializer_list<U> ilist,
             Args&&... args) ABSL_ATTRIBUTE_LIFETIME_BOUND {
    if (ok()) {
      this->Clear();
      this->MakeValue(ilist, std::forward<Args>(args)...);
    } else {
      this->MakeValue(ilist, std::forward<Args>(args)...);
      this->status_ = absl::OkStatus();
    }
    return this->data_;
  }

  // KeyOr<T>::AssignStatus()
  //
  // Sets the status of `absl::KeyOr<T>` to the given non-ok status value.
  //
  // NOTE: We recommend using the constructor and `operator=` where possible.
  // This method is intended for use in generic programming, to enable setting
  // the status of a `KeyOr<T>` when `T` may be `Status`. In that case, the
  // constructor and `operator=` would assign into the inner value of type
  // `Status`, rather than status of the `KeyOr` (b/280392796).
  //
  // REQUIRES: !Status(std::forward<U>(v)).ok(). This requirement is DCHECKed.
  // In optimized builds, passing absl::OkStatus() here will have the effect
  // of passing absl::StatusCode::kInternal as a fallback.
  using internal_KeyOr::KeyOrData<T>::AssignStatus;

 private:
  using internal_KeyOr::KeyOrData<T>::Assign;
  template <typename U>
  void Assign(const absl::KeyOr<U>& other);
  template <typename U>
  void Assign(absl::KeyOr<U>&& other);
};


template< typename V >
class Value : public KeyOr< absl::string_view, V > {};

// operator==()
//
// This operator checks the equality of two `absl::KeyOr<T>` objects.
template <typename T>
bool operator==(const KeyOr<T>& lhs, const KeyOr<T>& rhs) {
  if (lhs.ok() && rhs.ok()) return *lhs == *rhs;
  return lhs.status() == rhs.status();
}

// operator!=()
//
// This operator checks the inequality of two `absl::KeyOr<T>` objects.
template <typename T>
bool operator!=(const KeyOr<T>& lhs, const KeyOr<T>& rhs) {
  return !(lhs == rhs);
}

// Prints the `value` or the status in brackets to `os`.
//
// Requires `T` supports `operator<<`.  Do not rely on the output format which
// may change without notice.
template <typename T, typename std::enable_if<
                          absl::HasOstreamOperator<T>::value, int>::type = 0>
std::ostream& operator<<(std::ostream& os, const KeyOr<T>& status_or) {
  if (status_or.ok()) {
    os << status_or.value();
  } else {
    os << internal_KeyOr::StringifyRandom::OpenBrackets()
       << status_or.status()
       << internal_KeyOr::StringifyRandom::CloseBrackets();
  }
  return os;
}

// As above, but supports `StrCat`, `StrFormat`, etc.
//
// Requires `T` has `AbslStringify`.  Do not rely on the output format which
// may change without notice.
template <
    typename Sink, typename T,
    typename std::enable_if<absl::HasAbslStringify<T>::value, int>::type = 0>
void AbslStringify(Sink& sink, const KeyOr<T>& status_or) {
  if (status_or.ok()) {
    absl::Format(&sink, "%v", status_or.value());
  } else {
    absl::Format(&sink, "%s%v%s",
                 internal_KeyOr::StringifyRandom::OpenBrackets(),
                 status_or.status(),
                 internal_KeyOr::StringifyRandom::CloseBrackets());
  }
}

//------------------------------------------------------------------------------
// Implementation details for KeyOr<T>
//------------------------------------------------------------------------------

// TODO(sbenza): avoid the string here completely.
template <typename T>
KeyOr<T>::KeyOr() : Base(Status(absl::StatusCode::kUnknown, "")) {}

template <typename T>
template <typename U>
inline void KeyOr<T>::Assign(const KeyOr<U>& other) {
  if (other.ok()) {
    this->Assign(*other);
  } else {
    this->AssignStatus(other.status());
  }
}

template <typename T>
template <typename U>
inline void KeyOr<T>::Assign(KeyOr<U>&& other) {
  if (other.ok()) {
    this->Assign(*std::move(other));
  } else {
    this->AssignStatus(std::move(other).status());
  }
}
template <typename T>
template <typename... Args>
KeyOr<T>::KeyOr(absl::in_place_t, Args&&... args)
    : Base(absl::in_place, std::forward<Args>(args)...) {}

template <typename T>
template <typename U, typename... Args>
KeyOr<T>::KeyOr(absl::in_place_t, std::initializer_list<U> ilist,
                      Args&&... args)
    : Base(absl::in_place, ilist, std::forward<Args>(args)...) {}

template <typename T>
const Status& KeyOr<T>::status() const& {
  return this->status_;
}
template <typename T>
Status KeyOr<T>::status() && {
  return ok() ? OkStatus() : std::move(this->status_);
}

template <typename T>
const T& KeyOr<T>::value() const& {
  if (!this->ok()) internal_KeyOr::ThrowBadKeyOrAccess(this->status_);
  return this->data_;
}

template <typename T>
T& KeyOr<T>::value() & {
  if (!this->ok()) internal_KeyOr::ThrowBadKeyOrAccess(this->status_);
  return this->data_;
}

template <typename T>
const T&& KeyOr<T>::value() const&& {
  if (!this->ok()) {
    internal_KeyOr::ThrowBadKeyOrAccess(std::move(this->status_));
  }
  return std::move(this->data_);
}

template <typename T>
T&& KeyOr<T>::value() && {
  if (!this->ok()) {
    internal_KeyOr::ThrowBadKeyOrAccess(std::move(this->status_));
  }
  return std::move(this->data_);
}

template <typename T>
const T& KeyOr<T>::operator*() const& {
  this->EnsureOk();
  return this->data_;
}

template <typename T>
T& KeyOr<T>::operator*() & {
  this->EnsureOk();
  return this->data_;
}

template <typename T>
const T&& KeyOr<T>::operator*() const&& {
  this->EnsureOk();
  return std::move(this->data_);
}

template <typename T>
T&& KeyOr<T>::operator*() && {
  this->EnsureOk();
  return std::move(this->data_);
}

template <typename T>
absl::Nonnull<const T*> KeyOr<T>::operator->() const {
  this->EnsureOk();
  return &this->data_;
}

template <typename T>
absl::Nonnull<T*> KeyOr<T>::operator->() {
  this->EnsureOk();
  return &this->data_;
}

template <typename T>
template <typename U>
T KeyOr<T>::value_or(U&& default_value) const& {
  if (ok()) {
    return this->data_;
  }
  return std::forward<U>(default_value);
}

template <typename T>
template <typename U>
T KeyOr<T>::value_or(U&& default_value) && {
  if (ok()) {
    return std::move(this->data_);
  }
  return std::forward<U>(default_value);
}

template <typename T>
void KeyOr<T>::IgnoreError() const {
  // no-op
}

ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_STATUS_KeyOr_H_
