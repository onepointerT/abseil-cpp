// Copyright 2020 The Abseil Authors.
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
#ifndef ABSL_STATUS_INTERNAL_KeyOr_INTERNAL_H_
#define ABSL_STATUS_INTERNAL_KeyOr_INTERNAL_H_

#include <cstdint>
#include <type_traits>
#include <utility>

#include "absl/base/attributes.h"
#include "absl/base/nullability.h"
#include "absl/meta/type_traits.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "absl/utility/utility.h"

namespace absl {
ABSL_NAMESPACE_BEGIN

template <typename T, typename V>
class ABSL_MUST_USE_RESULT KeyOr;

namespace internal_KeyOr {

// Detects whether `U` has conversion operator to `KeyOr<T>`, i.e. `operator
// KeyOr<T>()`.
template <typename T, typename U, typename = void>
struct HasConversionOperatorToKeyOr : std::false_type {};

template <typename T, typename U>
void test(char (*)[sizeof(std::declval<U>().operator absl::KeyOr<T>())]);

template <typename T, typename U>
struct HasConversionOperatorToKeyOr<T, U, decltype(test<T, U>(0))>
    : std::true_type {};

// Detects whether `T` is constructible or convertible from `KeyOr<U>`.
template <typename T, typename U>
using IsConstructibleOrConvertibleFromKeyOr =
    absl::disjunction<std::is_constructible<T, KeyOr<U>&>,
                      std::is_constructible<T, const KeyOr<U>&>,
                      std::is_constructible<T, KeyOr<U>&&>,
                      std::is_constructible<T, const KeyOr<U>&&>,
                      std::is_convertible<KeyOr<U>&, T>,
                      std::is_convertible<const KeyOr<U>&, T>,
                      std::is_convertible<KeyOr<U>&&, T>,
                      std::is_convertible<const KeyOr<U>&&, T>>;

// Detects whether `T` is constructible or convertible or assignable from
// `KeyOr<U>`.
template <typename T, typename U>
using IsConstructibleOrConvertibleOrAssignableFromKeyOr =
    absl::disjunction<IsConstructibleOrConvertibleFromKeyOr<T, U>,
                      std::is_assignable<T&, KeyOr<U>&>,
                      std::is_assignable<T&, const KeyOr<U>&>,
                      std::is_assignable<T&, KeyOr<U>&&>,
                      std::is_assignable<T&, const KeyOr<U>&&>>;

// Detects whether direct initializing `KeyOr<T>` from `U` is ambiguous, i.e.
// when `U` is `KeyOr<V>` and `T` is constructible or convertible from `V`.
template <typename T, typename U>
struct IsDirectInitializationAmbiguous
    : public absl::conditional_t<
          std::is_same<absl::remove_cvref_t<U>, U>::value, std::false_type,
          IsDirectInitializationAmbiguous<T, absl::remove_cvref_t<U>>> {};

template <typename T, typename V>
struct IsDirectInitializationAmbiguous<T, absl::KeyOr<V>>
    : public IsConstructibleOrConvertibleFromKeyOr<T, V> {};

// Checks against the constraints of the direction initialization, i.e. when
// `KeyOr<T>::KeyOr(U&&)` should participate in overload resolution.
template <typename T, typename U>
using IsDirectInitializationValid = absl::disjunction<
    // Short circuits if T is basically U.
    std::is_same<T, absl::remove_cvref_t<U>>,
    absl::negation<absl::disjunction<
        std::is_same<absl::KeyOr<T>, absl::remove_cvref_t<U>>,
        std::is_same<absl::Status, absl::remove_cvref_t<U>>,
        std::is_same<absl::in_place_t, absl::remove_cvref_t<U>>,
        IsDirectInitializationAmbiguous<T, U>>>>;

// This trait detects whether `KeyOr<T>::operator=(U&&)` is ambiguous, which
// is equivalent to whether all the following conditions are met:
// 1. `U` is `KeyOr<V>`.
// 2. `T` is constructible and assignable from `V`.
// 3. `T` is constructible and assignable from `U` (i.e. `KeyOr<V>`).
// For example, the following code is considered ambiguous:
// (`T` is `bool`, `U` is `KeyOr<bool>`, `V` is `bool`)
//   KeyOr<bool> s1 = true;  // s1.ok() && s1.ValueOrDie() == true
//   KeyOr<bool> s2 = false;  // s2.ok() && s2.ValueOrDie() == false
//   s1 = s2;  // ambiguous, `s1 = s2.ValueOrDie()` or `s1 = bool(s2)`?
template <typename T, typename U>
struct IsForwardingAssignmentAmbiguous
    : public absl::conditional_t<
          std::is_same<absl::remove_cvref_t<U>, U>::value, std::false_type,
          IsForwardingAssignmentAmbiguous<T, absl::remove_cvref_t<U>>> {};

template <typename T, typename U>
struct IsForwardingAssignmentAmbiguous<T, absl::KeyOr<U>>
    : public IsConstructibleOrConvertibleOrAssignableFromKeyOr<T, U> {};

// Checks against the constraints of the forwarding assignment, i.e. whether
// `KeyOr<T>::operator(U&&)` should participate in overload resolution.
template <typename T, typename U>
using IsForwardingAssignmentValid = absl::disjunction<
    // Short circuits if T is basically U.
    std::is_same<T, absl::remove_cvref_t<U>>,
    absl::negation<absl::disjunction<
        std::is_same<absl::KeyOr<T>, absl::remove_cvref_t<U>>,
        std::is_same<absl::Status, absl::remove_cvref_t<U>>,
        std::is_same<absl::in_place_t, absl::remove_cvref_t<U>>,
        IsForwardingAssignmentAmbiguous<T, U>>>>;

template <bool Value, typename T>
using Equality = std::conditional_t<Value, T, absl::negation<T>>;

template <bool Explicit, typename T, typename U, bool Lifetimebound>
using IsConstructionValid = absl::conjunction<
    Equality<Lifetimebound,
             type_traits_internal::IsLifetimeBoundAssignment<T, U>>,
    IsDirectInitializationValid<T, U&&>, std::is_constructible<T, U&&>,
    Equality<!Explicit, std::is_convertible<U&&, T>>,
    absl::disjunction<
        std::is_same<T, absl::remove_cvref_t<U>>,
        absl::conjunction<
            std::conditional_t<
                Explicit,
                absl::negation<std::is_constructible<absl::Status, U&&>>,
                absl::negation<std::is_convertible<U&&, absl::Status>>>,
            absl::negation<
                internal_KeyOr::HasConversionOperatorToKeyOr<T, U&&>>>>>;

template <typename T, typename U, bool Lifetimebound>
using IsAssignmentValid = absl::conjunction<
    Equality<Lifetimebound,
             type_traits_internal::IsLifetimeBoundAssignment<T, U>>,
    std::is_constructible<T, U&&>, std::is_assignable<T&, U&&>,
    absl::disjunction<
        std::is_same<T, absl::remove_cvref_t<U>>,
        absl::conjunction<
            absl::negation<std::is_convertible<U&&, absl::Status>>,
            absl::negation<HasConversionOperatorToKeyOr<T, U&&>>>>,
    IsForwardingAssignmentValid<T, U&&>>;

template <bool Explicit, typename T, typename U>
using IsConstructionFromStatusValid = absl::conjunction<
    absl::negation<std::is_same<absl::KeyOr<T>, absl::remove_cvref_t<U>>>,
    absl::negation<std::is_same<T, absl::remove_cvref_t<U>>>,
    absl::negation<std::is_same<absl::in_place_t, absl::remove_cvref_t<U>>>,
    Equality<!Explicit, std::is_convertible<U, absl::Status>>,
    std::is_constructible<absl::Status, U>,
    absl::negation<HasConversionOperatorToKeyOr<T, U>>>;

template <bool Explicit, typename T, typename U, bool Lifetimebound,
          typename UQ>
using IsConstructionFromKeyOrValid = absl::conjunction<
    absl::negation<std::is_same<T, U>>,
    Equality<Lifetimebound,
             type_traits_internal::IsLifetimeBoundAssignment<T, U>>,
    std::is_constructible<T, UQ>,
    Equality<!Explicit, std::is_convertible<UQ, T>>,
    absl::negation<IsConstructibleOrConvertibleFromKeyOr<T, U>>>;

template <typename T, typename U, bool Lifetimebound>
using IsKeyOrAssignmentValid = absl::conjunction<
    absl::negation<std::is_same<T, absl::remove_cvref_t<U>>>,
    Equality<Lifetimebound,
             type_traits_internal::IsLifetimeBoundAssignment<T, U>>,
    std::is_constructible<T, U>, std::is_assignable<T, U>,
    absl::negation<IsConstructibleOrConvertibleOrAssignableFromKeyOr<
        T, absl::remove_cvref_t<U>>>>;

class Helper {
 public:
  // Move type-agnostic error handling to the .cc.
  static void HandleInvalidStatusCtorArg(absl::Nonnull<Status*>);
  [[noreturn]] static void Crash(const absl::Status& status);
};

// Construct an instance of T in `p` through placement new, passing Args... to
// the constructor.
// This abstraction is here mostly for the gcc performance fix.
template <typename T, typename... Args>
ABSL_ATTRIBUTE_NONNULL(1)
void PlacementNew(absl::Nonnull<void*> p, Args&&... args) {
  new (p) T(std::forward<Args>(args)...);
}

// Helper base class to hold the data and all operations.
// We move all this to a base class to allow mixing with the appropriate
// TraitsBase specialization.
template <typename T>
class KeyOrData {
  template <typename U>
  friend class KeyOrData;

 public:
  KeyOrData() = delete;

  KeyOrData(const KeyOrData& other) {
    if (other.ok()) {
      MakeValue(other.data_);
      MakeStatus();
    } else {
      MakeStatus(other.status_);
    }
  }

  KeyOrData(KeyOrData&& other) noexcept {
    if (other.ok()) {
      MakeValue(std::move(other.data_));
      MakeStatus();
    } else {
      MakeStatus(std::move(other.status_));
    }
  }

  template <typename U>
  explicit KeyOrData(const KeyOrData<U>& other) {
    if (other.ok()) {
      MakeValue(other.data_);
      MakeStatus();
    } else {
      MakeStatus(other.status_);
    }
  }

  template <typename U>
  explicit KeyOrData(KeyOrData<U>&& other) {
    if (other.ok()) {
      MakeValue(std::move(other.data_));
      MakeStatus();
    } else {
      MakeStatus(std::move(other.status_));
    }
  }

  template <typename... Args>
  explicit KeyOrData(absl::in_place_t, Args&&... args)
      : data_(std::forward<Args>(args)...) {
    MakeStatus();
  }

  explicit KeyOrData(const T& value) : data_(value) {
    MakeStatus();
  }
  explicit KeyOrData(T&& value) : data_(std::move(value)) {
    MakeStatus();
  }

  template <typename U,
            absl::enable_if_t<std::is_constructible<absl::Status, U&&>::value,
                              int> = 0>
  explicit KeyOrData(U&& v) : status_(std::forward<U>(v)) {
    EnsureNotOk();
  }

  KeyOrData& operator=(const KeyOrData& other) {
    if (this == &other) return *this;
    if (other.ok())
      Assign(other.data_);
    else
      AssignStatus(other.status_);
    return *this;
  }

  KeyOrData& operator=(KeyOrData&& other) {
    if (this == &other) return *this;
    if (other.ok())
      Assign(std::move(other.data_));
    else
      AssignStatus(std::move(other.status_));
    return *this;
  }

  ~KeyOrData() {
    if (ok()) {
      status_.~Status();
      data_.~T();
    } else {
      status_.~Status();
    }
  }

  template <typename U>
  void Assign(U&& value) {
    if (ok()) {
      data_ = std::forward<U>(value);
    } else {
      MakeValue(std::forward<U>(value));
      status_ = OkStatus();
    }
  }

  template <typename U>
  void AssignStatus(U&& v) {
    Clear();
    status_ = static_cast<absl::Status>(std::forward<U>(v));
    EnsureNotOk();
  }

  bool ok() const { return status_.ok(); }

 protected:
  // status_ will always be active after the constructor.
  // We make it a union to be able to initialize exactly how we need without
  // waste.
  // Eg. in the copy constructor we use the default constructor of Status in
  // the ok() path to avoid an extra Ref call.
  union {
    Status status_;
  };

  // data_ is active iff status_.ok()==true
  struct Dummy {};
  union {
    // When T is const, we need some non-const object we can cast to void* for
    // the placement new. dummy_ is that object.
    Dummy dummy_;
    T data_;
  };

  void Clear() {
    if (ok()) data_.~T();
  }

  void EnsureOk() const {
    if (ABSL_PREDICT_FALSE(!ok())) Helper::Crash(status_);
  }

  void EnsureNotOk() {
    if (ABSL_PREDICT_FALSE(ok())) Helper::HandleInvalidStatusCtorArg(&status_);
  }

  // Construct the value (ie. data_) through placement new with the passed
  // argument.
  template <typename... Arg>
  void MakeValue(Arg&&... arg) {
    internal_KeyOr::PlacementNew<T>(&dummy_, std::forward<Arg>(arg)...);
  }

  // Construct the status (ie. status_) through placement new with the passed
  // argument.
  template <typename... Args>
  void MakeStatus(Args&&... args) {
    internal_KeyOr::PlacementNew<Status>(&status_,
                                            std::forward<Args>(args)...);
  }
};

// Helper base classes to allow implicitly deleted constructors and assignment
// operators in `KeyOr`. For example, `CopyCtorBase` will explicitly delete
// the copy constructor when T is not copy constructible and `KeyOr` will
// inherit that behavior implicitly.
template <typename T, bool = std::is_copy_constructible<T>::value>
struct CopyCtorBase {
  CopyCtorBase() = default;
  CopyCtorBase(const CopyCtorBase&) = default;
  CopyCtorBase(CopyCtorBase&&) = default;
  CopyCtorBase& operator=(const CopyCtorBase&) = default;
  CopyCtorBase& operator=(CopyCtorBase&&) = default;
};

template <typename T>
struct CopyCtorBase<T, false> {
  CopyCtorBase() = default;
  CopyCtorBase(const CopyCtorBase&) = delete;
  CopyCtorBase(CopyCtorBase&&) = default;
  CopyCtorBase& operator=(const CopyCtorBase&) = default;
  CopyCtorBase& operator=(CopyCtorBase&&) = default;
};

template <typename T, bool = std::is_move_constructible<T>::value>
struct MoveCtorBase {
  MoveCtorBase() = default;
  MoveCtorBase(const MoveCtorBase&) = default;
  MoveCtorBase(MoveCtorBase&&) = default;
  MoveCtorBase& operator=(const MoveCtorBase&) = default;
  MoveCtorBase& operator=(MoveCtorBase&&) = default;
};

template <typename T>
struct MoveCtorBase<T, false> {
  MoveCtorBase() = default;
  MoveCtorBase(const MoveCtorBase&) = default;
  MoveCtorBase(MoveCtorBase&&) = delete;
  MoveCtorBase& operator=(const MoveCtorBase&) = default;
  MoveCtorBase& operator=(MoveCtorBase&&) = default;
};

template <typename T, bool = std::is_copy_constructible<T>::value&&
                          std::is_copy_assignable<T>::value>
struct CopyAssignBase {
  CopyAssignBase() = default;
  CopyAssignBase(const CopyAssignBase&) = default;
  CopyAssignBase(CopyAssignBase&&) = default;
  CopyAssignBase& operator=(const CopyAssignBase&) = default;
  CopyAssignBase& operator=(CopyAssignBase&&) = default;
};

template <typename T>
struct CopyAssignBase<T, false> {
  CopyAssignBase() = default;
  CopyAssignBase(const CopyAssignBase&) = default;
  CopyAssignBase(CopyAssignBase&&) = default;
  CopyAssignBase& operator=(const CopyAssignBase&) = delete;
  CopyAssignBase& operator=(CopyAssignBase&&) = default;
};

template <typename T, bool = std::is_move_constructible<T>::value&&
                          std::is_move_assignable<T>::value>
struct MoveAssignBase {
  MoveAssignBase() = default;
  MoveAssignBase(const MoveAssignBase&) = default;
  MoveAssignBase(MoveAssignBase&&) = default;
  MoveAssignBase& operator=(const MoveAssignBase&) = default;
  MoveAssignBase& operator=(MoveAssignBase&&) = default;
};

template <typename T>
struct MoveAssignBase<T, false> {
  MoveAssignBase() = default;
  MoveAssignBase(const MoveAssignBase&) = default;
  MoveAssignBase(MoveAssignBase&&) = default;
  MoveAssignBase& operator=(const MoveAssignBase&) = default;
  MoveAssignBase& operator=(MoveAssignBase&&) = delete;
};

[[noreturn]] void ThrowBadKeyOrAccess(absl::Status status);

// Used to introduce jitter into the output of printing functions for
// `KeyOr` (i.e. `AbslStringify` and `operator<<`).
class StringifyRandom {
  enum BracesType {
    kBareParens = 0,
    kSpaceParens,
    kBareBrackets,
    kSpaceBrackets,
  };

  // Returns a random `BracesType` determined once per binary load.
  static BracesType RandomBraces() {
    static const BracesType kRandomBraces = static_cast<BracesType>(
        (reinterpret_cast<uintptr_t>(&kRandomBraces) >> 4) % 4);
    return kRandomBraces;
  }

 public:
  static inline absl::string_view OpenBrackets() {
    switch (RandomBraces()) {
      case kBareParens:
        return "(";
      case kSpaceParens:
        return "( ";
      case kBareBrackets:
        return "[";
      case kSpaceBrackets:
        return "[ ";
    }
    return "(";
  }

  static inline absl::string_view CloseBrackets() {
    switch (RandomBraces()) {
      case kBareParens:
        return ")";
      case kSpaceParens:
        return " )";
      case kBareBrackets:
        return "]";
      case kSpaceBrackets:
        return " ]";
    }
    return ")";
  }
};

}  // namespace internal_KeyOr
ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_STATUS_INTERNAL_KeyOr_INTERNAL_H_
