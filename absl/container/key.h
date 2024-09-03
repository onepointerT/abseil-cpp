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
// File: key.h
// -----------------------------------------------------------------------------
//
// This header file defines the Abseil `key` library, consisting of:
//
//   * An `absl::Key` class for holding an Key for <Key, Value> maps
//   * A simple error handling on Keys
//   * A set of canonical `absl::KeyStatusCode` error codes, and associated
//     utilities for generating and propagating key status codes.
//   * A set of helper functions for creating status codes and checking their
//     values
//
// Within Google, `absl::Key` is the primary mechanism for communicating
// Keys in C++, and is used to represent non-error state in both in-process
// library calls as well as normal calls on the container. A `absl::Key` can
// be inherited for compliance of comparators like `std::less` and error code
// checking like with `Key::status::has_Value()`, `Key::status::unchanged()`
// and similar functionalities of key and its status with a in-container type.
// In similar implementations it is possible to have a KeyOr<T> convenience
// behaviour so that a set or a vector is a set of comparable objects or a key.
//
// Example:
//
// absl::Key<T> myFunction(absl::string_view fname, ...) {
//   // TODO
// }
//
// An `absl::Key` is designed to either return "OK" or one of a number of
// different error codes, corresponding to typical error conditions.
// In almost all cases, when using `absl::Key` you should use the canonical
// error codes (of type `absl::KeyStatusCode`) enumerated in this header file.
// These canonical codes are understood across the codebase and will be
// accepted across all API and RPC boundaries.
#ifndef ABSL_STATUS_STATUS_H_
#define ABSL_STATUS_STATUS_H_

#include <cassert>
#include <cstdint>
#include <ostream>
#include <span>
#include <string>
#include <utility>

#include "absl/base/attributes.h"
#include "absl/base/config.h"
#include "absl/base/macros.h"
#include "absl/base/nullability.h"
#include "absl/base/optimization.h"
#include "absl/container/internal/key_internal.h"
#include "absl/base/optimization.h"
#include "absl/strings/cord.h"
#include "absl/strings/string_view.h"
#include "absl/types/optional.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


template< typename KeyT >
class KeyComparable;

template< typename KeyT, typename ValueT >
class KeyValuePair;

// absl::KeyStatusCode
//
// An `absl::KeyStatusCode` is an enumerated type indicating either no error ("OK"),
// an error condition or indicators on a key's corresponding value.
// In most cases, an `absl::KeyStatus` indicates the status of values inside of a
// container, errors on the key's position and the value's usability.
// 
// The errors listed below are the canonical errors associated with
// `absl::Key` and are used throughout the codebase. As a result, these
// error codes are somewhat generic.
//
// In general, try to return the most specific error that applies if more than
// one error may pertain. For example, prefer `kOutOfRange` over
// `kFailedPrecondition` if both codes apply. Similarly prefer `kNotFound` or
// `kAlreadyExists` over `kFailedPrecondition`.
//
// Because these errors may cross RPC boundaries, these codes are tied to the
// `google.rpc.Code` definitions within
// https://github.com/googleapis/googleapis/blob/master/google/rpc/code.proto
// The string value of these RPC codes is denoted within each enum below.
//
// If your error handling code requires more context, you can attach payloads
// to your status. See `absl::Key::SetPayload()` and
// `absl::Key::GetPayload()` below.
enum class KeyStatusCode : int {
  // KeyStatusCode::kOk
  //
  // kOK (gRPC code "OK") does not indicate an error; this value is returned on
  // success. It is typical to check for this value before proceeding on any
  // given call across an API or RPC boundary. To check this value, use the
  // `absl::Key::ok()` member function rather than inspecting the raw code.
  kOk = 0,

  // KeyStatusCode::kCancelled
  //
  // kCancelled (gRPC code "CANCELLED") indicates the operation was cancelled,
  // typically by the caller.
  kCancelled = 1,

  // KeyStatusCode::kUnknown
  //
  // kUnknown (gRPC code "UNKNOWN") indicates an unknown error occurred. In
  // general, more specific errors should be raised, if possible. Errors raised
  // by APIs that do not return enough error information may be converted to
  // this error.
  kUnknown = 2,

  // KeyStatusCode::kInvalidArgument
  //
  // kInvalidArgument (gRPC code "INVALID_ARGUMENT") indicates the caller
  // specified an invalid argument, such as a malformed filename. Note that use
  // of such errors should be narrowly limited to indicate the invalid nature of
  // the arguments themselves. Errors with validly formed arguments that may
  // cause errors with the state of the receiving system should be denoted with
  // `kFailedPrecondition` instead.
  kInvalidArgument = 3,

  // KeyStatusCode::kDeadlineExceeded
  //
  // kDeadlineExceeded (gRPC code "DEADLINE_EXCEEDED") indicates a deadline
  // expired before the operation could complete. For operations that may change
  // state within a system, this error may be returned even if the operation has
  // completed successfully. For example, a successful response from a server
  // could have been delayed long enough for the deadline to expire.
  kDeadlineExceeded = 4,

  // KeyStatusCode::kNotFound
  //
  // kNotFound (gRPC code "NOT_FOUND") indicates some requested entity (such as
  // a file or directory) was not found.
  //
  // `kNotFound` is useful if a request should be denied for an entire class of
  // users, such as during a gradual feature rollout or undocumented allow list.
  // If a request should be denied for specific sets of users, such as through
  // user-based access control, use `kPermissionDenied` instead.
  kNotFound = 5,

  // KeyStatusCode::kAlreadyExists
  //
  // kAlreadyExists (gRPC code "ALREADY_EXISTS") indicates that the entity a
  // caller attempted to create (such as a file or directory) is already
  // present.
  kAlreadyExists = 6,

  // KeyStatusCode::kPermissionDenied
  //
  // kPermissionDenied (gRPC code "PERMISSION_DENIED") indicates that the caller
  // does not have permission to execute the specified operation. Note that this
  // error is different than an error due to an *un*authenticated user. This
  // error code does not imply the request is valid or the requested entity
  // exists or satisfies any other pre-conditions.
  //
  // `kPermissionDenied` must not be used for rejections caused by exhausting
  // some resource. Instead, use `kResourceExhausted` for those errors.
  // `kPermissionDenied` must not be used if the caller cannot be identified.
  // Instead, use `kUnauthenticated` for those errors.
  kPermissionDenied = 7,

  // KeyStatusCode::kResourceExhausted
  //
  // kResourceExhausted (gRPC code "RESOURCE_EXHAUSTED") indicates some resource
  // has been exhausted, perhaps a per-user quota, or perhaps the entire file
  // system is out of space.
  kResourceExhausted = 8,

  // KeyStatusCode::kFailedPrecondition
  //
  // kFailedPrecondition (gRPC code "FAILED_PRECONDITION") indicates that the
  // operation was rejected because the system is not in a state required for
  // the operation's execution. For example, a directory to be deleted may be
  // non-empty, an "rmdir" operation is applied to a non-directory, etc.
  //
  // Some guidelines that may help a service implementer in deciding between
  // `kFailedPrecondition`, `kAborted`, and `kUnavailable`:
  //
  //  (a) Use `kUnavailable` if the client can retry just the failing call.
  //  (b) Use `kAborted` if the client should retry at a higher transaction
  //      level (such as when a client-specified test-and-set fails, indicating
  //      the client should restart a read-modify-write sequence).
  //  (c) Use `kFailedPrecondition` if the client should not retry until
  //      the system state has been explicitly fixed. For example, if a "rmdir"
  //      fails because the directory is non-empty, `kFailedPrecondition`
  //      should be returned since the client should not retry unless
  //      the files are deleted from the directory.
  kFailedPrecondition = 9,

  // KeyStatusCode::kAborted
  //
  // kAborted (gRPC code "ABORTED") indicates the operation was aborted,
  // typically due to a concurrency issue such as a sequencer check failure or a
  // failed transaction.
  //
  // See the guidelines above for deciding between `kFailedPrecondition`,
  // `kAborted`, and `kUnavailable`.
  kAborted = 10,

  // KeyStatusCode::kOutOfRange
  //
  // kOutOfRange (gRPC code "OUT_OF_RANGE") indicates the operation was
  // attempted past the valid range, such as seeking or reading past an
  // end-of-file.
  //
  // Unlike `kInvalidArgument`, this error indicates a problem that may
  // be fixed if the system state changes. For example, a 32-bit file
  // system will generate `kInvalidArgument` if asked to read at an
  // offset that is not in the range [0,2^32-1], but it will generate
  // `kOutOfRange` if asked to read from an offset past the current
  // file size.
  //
  // There is a fair bit of overlap between `kFailedPrecondition` and
  // `kOutOfRange`.  We recommend using `kOutOfRange` (the more specific
  // error) when it applies so that callers who are iterating through
  // a space can easily look for an `kOutOfRange` error to detect when
  // they are done.
  kOutOfRange = 11,

  // KeyStatusCode::kUnimplemented
  //
  // kUnimplemented (gRPC code "UNIMPLEMENTED") indicates the operation is not
  // implemented or supported in this service. In this case, the operation
  // should not be re-attempted.
  kUnimplemented = 12,

  // KeyStatusCode::kInternal
  //
  // kInternal (gRPC code "INTERNAL") indicates an internal error has occurred
  // and some invariants expected by the underlying system have not been
  // satisfied. This error code is reserved for serious errors.
  kInternal = 13,

  // KeyStatusCode::kUnavailable
  //
  // kUnavailable (gRPC code "UNAVAILABLE") indicates the service is currently
  // unavailable and that this is most likely a transient condition. An error
  // such as this can be corrected by retrying with a backoff scheme. Note that
  // it is not always safe to retry non-idempotent operations.
  //
  // See the guidelines above for deciding between `kFailedPrecondition`,
  // `kAborted`, and `kUnavailable`.
  kUnavailable = 14,

  // KeyStatusCode::kDataLoss
  //
  // kDataLoss (gRPC code "DATA_LOSS") indicates that unrecoverable data loss or
  // corruption has occurred. As this error is serious, proper alerting should
  // be attached to errors such as this.
  kDataLoss = 15,

  // KeyStatusCode::kUnauthenticated
  //
  // kUnauthenticated (gRPC code "UNAUTHENTICATED") indicates that the request
  // does not have valid authentication credentials for the operation. Correct
  // the authentication and try again.
  kUnauthenticated = 16,

  // KeyStatusCode::kChanged
  //
  // kUnchanged indicates if a value was unchanged since the last access to the
  // container or the last access to the element. The default implementation
  // defaults to last access of the element. Needs to be set on changin of the
  // value of an key and reset on recognizing or working on the change.
  kChanged = 17,

  // KeyStatusCode::kValueUntouched
  //
  // kValueUntouched indicates if a value (this means the class, struct or typename's
  // value) was not changed since the last lookup.
  kValueUntouched = 18,

  // KeyStatusCode::kValueForKeyAvailable
  //
  // kValueForKeyAvailable indicated if an element was found in the container
  // for this key. In some usecases, e.ex. when awaiting elements, a categoristic
  // (key, empty-value) container may be initialized until the element arrives.
  kValueForKeyAvailable = 19,
  kValueForKeyUnavailable = 20,

  // KeyStatusCode::DoNotUseReservedForFutureExpansionUseDefaultInSwitchInstead_
  //
  // NOTE: this error code entry should not be used and you should not rely on
  // its value, which may change.
  //
  // The purpose of this enumerated value is to force people who handle status
  // codes with `switch()` statements to *not* simply enumerate all possible
  // values, but instead provide a "default:" case. Providing such a default
  // case ensures that code will compile when new codes are added.
  kDoNotUseReservedForFutureExpansionUseDefaultInSwitchInstead_ = 21
};

// KeyStatusCodeToString()
//
// Returns the name for the status code, or "" if it is an unknown value.
std::string KeyStatusCodeToString(KeyStatusCode code);

// operator<<
//
// Streams KeyStatusCodeToString(code) to `os`.
std::ostream& operator<<(std::ostream& os, KeyStatusCode code);


// absl::KeyStatus
//
// The `absl::Key` class is generally used to gracefully find errors and
// information on container values and their keys across API boundaries.
// Some of these errors may be recoverable, but others may not. Most
// functions which can use a `absl::Key` should be designed to return
// either an `absl::Key<T>` (or the similar `absl::KeyOr<T>`, which holds
// either an object of type `T` or an error).
//
// API developers should construct their functions to return `absl::OkKey()`
// upon success, or an `absl::KeyStatusCode` upon another type of lookup (e.g
// an `absl::KeyStatusCode::kUnavailable` error). The API provides convenience
// functions to construct each status code.
//
// Example:
//
// absl::Key myFunction(absl::string_view fname, ...) {
//   ...
//   // encounter error
//   if (error condition) {
//     // Construct an absl::KeyStatusCode::kInvalidArgument error
//     return absl::InvalidArgumentError("bad mode");
//   }
//   // else, return OK
//   return absl::OkStatus();
// }
//
// Users handling status error codes should prefer checking for an OK status
// using the `ok()` member function. Handling multiple error codes may justify
// use of switch statement, but only check for error codes you know how to
// handle; do not try to exhaustively match against all canonical error codes.
// Errors that cannot be handled should be logged and/or propagated for higher
// levels to deal with. If you do use a switch statement, make sure that you
// also provide a `default:` switch case, so that code does not break as other
// canonical codes are added to the API.
//
// Example:
//
//   absl::Key result = DoSomething();
//   if (!result.ok()) {
//     LOG(ERROR) << result;
//   }
//
//   // Provide a default if switching on multiple error codes
//   switch (result.code()) {
//     // The user hasn't authenticated. Ask them to reauth
//     case absl::KeyStatusCode::kUnauthenticated:
//       DoReAuth();
//       break;
//     // The user does not have permission. Log an error.
//     case absl::KeyStatusCode::kPermissionDenied:
//       LOG(ERROR) << result;
//       break;
//     // Propagate the error otherwise.
//     default:
//       return true;
//   }
//
// An `absl::Key` can unoptionally include more information like a `key_t` of a map
// and more information about a error or a status code. Typically, this serves one
// of several purposes:
//
//   * It may provide more fine-grained semantic information about an error, a status
//     code or the `absl::Key<T>` to facilitate actionable remedies.
//   * It may provide human-readable contextual information that is more
//     appropriate to display to an end user.
//   * It may provide comparable keys that are able to identify value objects and hold
//     a status code upon their corresponding value.
//
// Example:
//
//   absl::Key result = map->begin()->first;
//   // Inform user to retry after 30 seconds
//   // See more error details in googleapis/google/rpc/error_details.proto
//   if (absl::IsValueUnchanged(result)) {
//     google::rpc::RetryInfo info;
//     info.retry_delay().seconds() = 30;
//     // Payloads require a unique key (a URL to ensure no collisions with
//     // other payloads), and an `absl::Cord` to hold the encoded data.
//     absl::string_view info_msg = "The value changed.";
//     result.SetPayload(info_msg, info.SerializeAsCord());
//     return result;
//   }
//
// For documentation see https://abseil.io/docs/cpp/guides/key.
//
// Returned Key objects may not be ignored. key_internal.h has a forward
// declaration of the form
// class ABSL_MUST_USE_RESULT Key;
class ABSL_ATTRIBUTE_TRIVIAL_ABI Key {
 public:
  // Status indicator for the value
  KeyStatusCode status_value = KeyStatusCode::kValueForKeyUnavailable;
  // Status indicator for all-over (key, value) pair
  KeyStatusCode status_value_pair = KeyStatusCode::kInvalidArgument;
  
  // Constructors

  // This default constructor creates an OK status with no message or payload.
  // Avoid this constructor and prefer explicit construction of an OK status
  // with `absl::OkStatus()`.
  Key();

  // Creates a status in the canonical error space with the specified
  // `absl::KeyStatusCode` and error message.  If `code == absl::KeyStatusCode::kOk`,  // NOLINT
  // `msg` is ignored and an object identical to an OK status is constructed.
  //
  // The `msg` string must be in UTF-8. The implementation may complain (e.g.,  // NOLINT
  // by printing a warning) if it is not.
  Key(absl::KeyStatusCode code, absl::string_view msg);

  Key(const Key&);
  Key& operator=(const Key& x);

  // Move operators

  // The moved-from state is valid but unspecified.
  Key(Key&&) noexcept;
  Key& operator=(Key&&) noexcept;

  ~Key();

  // Some incidens may modify the status code for the key or the
  // (Key, value) pair, e.g. when a value gets swapped, available,
  // changed or a key got touched, the public properties will be modified
  // to a specific value.
  void keyChanged();
  void valueNew();
  void valueChanged();
  void valueUnmodified();
  void keyUnmodified();
  void valueUnknown();
  void setStatusValueProperty(const KeyStatusCode status);
  void setStatusKeyValueProperty(const KeyStatusCode status);

  // Key::Update()
  //
  // Updates the existing status with `new_status` provided that `this->ok()`.
  // If the existing status already contains a non-OK error, this update has no
  // effect and preserves the current data. Note that this behavior may change
  // in the future to augment a current non-ok status with additional
  // information about `new_status`.
  //
  // `Update()` provides a convenient way of keeping track of the first error
  // encountered.
  //
  // Example:
  //   // Instead of "if (overall_status.ok()) overall_status = new_status"
  //   overall_status.Update(new_status);
  //
  void Update(const Key& new_status);
  void Update(Key&& new_status);

  // Key::ok()
  //
  // Returns `true` if `this->code()` == `absl::KeyStatusCode::kOk`,
  // indicating the absence of an error.
  // Prefer checking for an OK status using this member function.
  ABSL_MUST_USE_RESULT bool ok() const;

  // Key::code()
  //
  // Returns the canonical error code of type `absl::KeyStatusCode` of this status.
  absl::KeyStatusCode code() const;

  // Key::raw_code()
  //
  // Returns a raw (canonical) error code corresponding to the enum value of
  // `google.rpc.Code` definitions within
  // https://github.com/googleapis/googleapis/blob/master/google/rpc/code.proto.
  // These values could be out of the range of canonical `absl::KeyStatusCode`
  // enum values.
  //
  // NOTE: This function should only be called when converting to an associated
  // wire format. Use `Key::code()` for error handling.
  int raw_code() const;

  // Key::message()
  //
  // Returns the error message associated with this error code, if available.
  // Note that this message rarely describes the error code.  It is not unusual
  // for the error message to be the empty string. As a result, prefer
  // `operator<<` or `Key::ToString()` for debug logging.
  absl::string_view message() const;

  friend bool operator==(const Key&, const Key&);
  friend bool operator!=(const Key&, const Key&);

  // Key::ToString()
  //
  // Returns a string based on the `mode`. By default, it returns combination of
  // the error code name, the message and any associated payload messages. This
  // string is designed simply to be human readable and its exact format should
  // not be load bearing. Do not depend on the exact format of the result of
  // `ToString()` which is subject to change.
  //
  // The printed code name and the message are generally substrings of the
  // result, and the payloads to be printed use the status payload printer
  // mechanism (which is internal).
  std::string ToString(
      KeyStatusToStringMode mode = KeyStatusToStringMode::kDefault) const;

  // Support `absl::StrCat`, `absl::StrFormat`, etc.
  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Key& status) {
    sink.Append(status.ToString(KeyStatusToStringMode::kWithEverything));
  }

  // Key::IgnoreError()
  //
  // Ignores any errors. This method does nothing except potentially suppress
  // complaints from any tools that are checking that errors are not dropped on
  // the floor.
  void IgnoreError() const;

  // swap()
  //
  // Swap the contents of one status with another.
  friend void swap(Key& a, Key& b) noexcept;

  //----------------------------------------------------------------------------
  // Payload Management APIs
  //----------------------------------------------------------------------------

  // A payload may be attached to a status to provide additional context to an
  // error that may not be satisfied by an existing `absl::KeyStatusCode`.
  // Typically, this payload serves one of several purposes:
  //
  //   * It may provide more fine-grained semantic information about the error
  //     to facilitate actionable remedies.
  //   * It may provide human-readable contextual information that is more
  //     appropriate to display to an end user.
  //
  // A payload consists of a [key,value] pair, where the key is a string
  // referring to a unique "type URL" and the value is an object of type
  // `absl::Cord` to hold the contextual data.
  //
  // The "type URL" should be unique and follow the format of a URL
  // (https://en.wikipedia.org/wiki/URL) and, ideally, provide some
  // documentation or schema on how to interpret its associated data. For
  // example, the default type URL for a protobuf message type is
  // "type.googleapis.com/packagename.messagename". Other custom wire formats
  // should define the format of type URL in a similar practice so as to
  // minimize the chance of conflict between type URLs.
  // Users should ensure that the type URL can be mapped to a concrete
  // C++ type if they want to deserialize the payload and read it effectively.
  //
  // To attach a payload to a status object, call `Key::SetPayload()`,
  // passing it the type URL and an `absl::Cord` of associated data. Similarly,
  // to extract the payload from a status, call `Key::GetPayload()`. You
  // may attach multiple payloads (with differing type URLs) to any given
  // status object, provided that the status is currently exhibiting an error
  // code (i.e. is not OK).

  // Key::GetPayload()
  //
  // Gets the payload of a status given its unique `type_url` key, if present.
  absl::optional<absl::Cord> GetPayload(absl::string_view type_url) const;

  // Key::SetPayload()
  //
  // Sets the payload for a non-ok status using a `type_url` key, overwriting
  // any existing payload for that `type_url`.
  //
  // NOTE: This function does nothing if the Key is ok.
  void SetPayload(absl::string_view type_url, absl::Cord payload);

  // Key::ErasePayload()
  //
  // Erases the payload corresponding to the `type_url` key.  Returns `true` if
  // the payload was present.
  bool ErasePayload(absl::string_view type_url);

  operator std::basic_string<char, std::char_traits<char>>() const;

  // Key::ForEachPayload()
  //
  // Iterates over the stored payloads and calls the
  // `visitor(type_key, payload)` callable for each one.
  //
  // NOTE: The order of calls to `visitor()` is not specified and may change at
  // any time.
  //
  // NOTE: Any mutation on the same 'absl::Key' object during visitation is
  // forbidden and could result in undefined behavior.
  void ForEachPayload(
      absl::FunctionRef<void(absl::string_view, const absl::Cord&)> visitor)
      const;

 private:
  friend Key CancelledError();

  // Creates a status in the canonical error space with the specified
  // code, and an empty error message.
  explicit Key(absl::KeyStatusCode code);

  // Underlying constructor for status from a rep_.
  explicit Key(uintptr_t rep) : rep_(rep) {}

  static void Ref(uintptr_t rep);
  static void Unref(uintptr_t rep);

  // REQUIRES: !ok()
  // Ensures rep is not inlined or shared with any other Key.
  static absl::Nonnull<key_internal::KeyStatusRep*> PrepareToModify(
      uintptr_t rep);

  // MSVC 14.0 limitation requires the const.
  static constexpr const char kMovedFromString[] =
      "Key accessed after move.";

  static absl::Nonnull<const std::string*> EmptyString();
  static absl::Nonnull<const std::string*> MovedFromString();

  // Returns whether rep contains an inlined representation.
  // See rep_ for details.
  static constexpr bool IsInlined(uintptr_t rep);

  // Indicates whether this Key was the rhs of a move operation. See rep_
  // for details.
  static constexpr bool IsMovedFrom(uintptr_t rep);
  static constexpr uintptr_t MovedFromRep();

  // Convert between error::Code and the inlined uintptr_t representation used
  // by rep_. See rep_ for details.
  static constexpr uintptr_t CodeToInlinedRep(absl::KeyStatusCode code);
  static constexpr absl::KeyStatusCode InlinedRepToCode(uintptr_t rep);

  // Converts between KeyStatusRep* and the external uintptr_t representation used
  // by rep_. See rep_ for details.
  static uintptr_t PointerToRep(key_internal::KeyStatusRep* r);
  static absl::Nonnull<const key_internal::KeyStatusRep*> RepToPointer(
      uintptr_t r);

  static std::string ToStringSlow(uintptr_t rep, KeyStatusToStringMode mode);

  // Key supports two different representations.
  //  - When the low bit is set it is an inlined representation.
  //    It uses the canonical error space, no message or payload.
  //    The error code is (rep_ >> 2).
  //    The (rep_ & 2) bit is the "moved from" indicator, used in IsMovedFrom().
  //  - When the low bit is off it is an external representation.
  //    In this case all the data comes from a heap allocated Rep object.
  //    rep_ is a key_internal::KeyStatusRep* pointer to that structure.
  uintptr_t rep_;

  friend class key_internal::KeyStatusRep;
};



template< typename KeyT >
class KeyComparable
  : public Key
  , public KeyT
{};


template< typename KeyT, typename ValueT >
class KeyValuePair
  : public KeyComparable< KeyT >
  , public std::span< ValueT >
{
public:
  void reset( ValueT value ) {
    std::span< ValueT >::operator[](0) = value;
  }
  
  void set_Value( ValueT value ) {
    this->valueNew();
    this->reset( value );
  }
};

// OkStatus()
//
// Returns an OK status, equivalent to a default constructed instance. Prefer
// usage of `absl::OkStatus()` when constructing such an OK status.
Key OkStatus();

// operator<<()
//
// Prints a human-readable representation of `x` to `os`.
std::ostream& operator<<(std::ostream& os, const Key& x);

// IsAborted()
// IsAlreadyExists()
// IsCancelled()
// IsDataLoss()
// IsDeadlineExceeded()
// IsFailedPrecondition()
// IsInternal()
// IsInvalidArgument()
// IsNotFound()
// IsOutOfRange()
// IsPermissionDenied()
// IsResourceExhausted()
// IsUnauthenticated()
// IsUnavailable()
// IsUnimplemented()
// IsUnknown()
// IsChanged()
// IsValueUntouched()
// IsValueForKeyAvailable()
//
// These convenience functions return `true` if a given status matches the
// `absl::KeyStatusCode` error code of its associated function.
ABSL_MUST_USE_RESULT bool IsAborted(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsAlreadyExists(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsCancelled(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsDataLoss(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsDeadlineExceeded(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsFailedPrecondition(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsInternal(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsInvalidArgument(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsNotFound(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsOutOfRange(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsPermissionDenied(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsResourceExhausted(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsUnauthenticated(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsUnavailable(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsUnimplemented(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsUnknown(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsChanged(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsValueUntouched(const KeyStatus& status);
ABSL_MUST_USE_RESULT bool IsValueForKeyAvailable(const KeyStatus& status);

// AbortedError()
// AlreadyExistsError()
// CancelledError()
// DataLossError()
// DeadlineExceededError()
// FailedPreconditionError()
// InternalError()
// InvalidArgumentError()
// NotFoundError()
// OutOfRangeError()
// PermissionDeniedError()
// ResourceExhaustedError()
// UnauthenticatedError()
// UnavailableError()
// UnimplementedError()
// UnknownError()
// IsChanged()
// IsValueUntouched()
// IsValueForKeyAvailable()
//
// These convenience functions create an `absl::Key` object with an error
// code as indicated by the associated function name, using the error message
// passed in `message`.
KeyStatus AbortedError(absl::string_view message);
KeyStatus AlreadyExistsError(absl::string_view message);
KeyStatus CancelledError(absl::string_view message);
KeyStatus DataLossError(absl::string_view message);
KeyStatus DeadlineExceededError(absl::string_view message);
KeyStatus FailedPreconditionError(absl::string_view message);
KeyStatus InternalError(absl::string_view message);
KeyStatus InvalidArgumentError(absl::string_view message);
KeyStatus NotFoundError(absl::string_view message);
KeyStatus OutOfRangeError(absl::string_view message);
KeyStatus PermissionDeniedError(absl::string_view message);
KeyStatus ResourceExhaustedError(absl::string_view message);
KeyStatus UnauthenticatedError(absl::string_view message);
KeyStatus UnavailableError(absl::string_view message);
KeyStatus UnimplementedError(absl::string_view message);
KeyStatus UnknownError(absl::string_view message);
KeyStatus Changed(absl::string_view message);
KeyStatus ValueUntouched(absl::string_view message);
KeyStatus ValueForKeyAvailable(absl::string_view message);

// ErrnoToKeyStatusCode()
//
// Returns the KeyStatusCode for `error_number`, which should be an `errno` value.
// See https://en.cppreference.com/w/cpp/error/errno_macros and similar
// references.
absl::KeyStatusCode ErrnoToKeyStatusCode(int error_number);

// ErrnoToStatus()
//
// Convenience function that creates a `absl::Key` using an `error_number`,
// which should be an `errno` value.
KeyStatus ErrnoToStatus(int error_number, absl::string_view message);

//------------------------------------------------------------------------------
// Implementation details follow
//------------------------------------------------------------------------------

inline Key::Key() : Key(absl::KeyStatusCode::kOk) {}

inline Key::Key(absl::KeyStatusCode code) : Key(CodeToInlinedRep(code)) {}

inline Key::Key(const Key& x) : Key(x.rep_) { Ref(rep_); }

inline Key& Key::operator=(const Key& x) {
  uintptr_t old_rep = rep_;
  if (x.rep_ != old_rep) {
    Ref(x.rep_);
    rep_ = x.rep_;
    Unref(old_rep);
  }
  return *this;
}

inline Key::Key(Key&& x) noexcept : Key(x.rep_) {
  x.rep_ = MovedFromRep();
}

inline Key& Key::operator=(Key&& x) noexcept {
  uintptr_t old_rep = rep_;
  if (x.rep_ != old_rep) {
    rep_ = x.rep_;
    x.rep_ = MovedFromRep();
    Unref(old_rep);
  }
  return *this;
}


inline void Key::keyChanged() {
  this->status_value = KeyStatusCode::kChanged;
}


inline void Key::valueNew() {
  this->status_value = KeyStatusCode::kChanged;
  this->status_value_pair = KeyStatusCode::kValueUntouched;
}


inline void Key::valueChanged() {
  this->status_value = KeyStatusCode::kChanged;
  this->status_value_pair = KeyStatusCode::kValueForKeyAvailable;
}


inline void Key::valueUnmodified() {
  this->status_value = KeyStatusCode::kValueUntouched;
  this->status_value_pair = KeyStatusCode::kOk;
}


inline void Key::keyUnmodified() {
  this->status_value = KeyStatusCode::kOk;
}


inline void Key::valueUnknown() {
  this->status_value = KeyStatusCode::kUnknown;
  this->status_value_pair = KeyStatusCode::kValueForKeyUnavailable;
}

inline void Key::setStatusValueProperty(const KeyStatusCode status) {
  this->status_value = status;
}

inline void Key::setStatusKeyValueProperty(const KeyStatusCode status) {
  this->status_value_pair = status;
}

inline void Key::Update(const Key& new_status) {
  if (ok()) {
    *this = new_status;
  }
}

inline void Key::Update(Key&& new_status) {
  if (ok()) {
    *this = std::move(new_status);
  }
}

inline Key::~Key() { Unref(rep_); }

inline bool Key::ok() const {
  return rep_ == CodeToInlinedRep(absl::KeyStatusCode::kOk);
}

inline absl::KeyStatusCode Key::code() const {
  return key_internal::MapToLocalCode(raw_code());
}

inline int Key::raw_code() const {
  if (IsInlined(rep_)) return static_cast<int>(InlinedRepToCode(rep_));
  return static_cast<int>(RepToPointer(rep_)->code());
}

inline absl::string_view Key::message() const {
  return !IsInlined(rep_)
             ? RepToPointer(rep_)->message()
             : (IsMovedFrom(rep_) ? absl::string_view(kMovedFromString)
                                  : absl::string_view());
}

inline bool operator==(const Key& lhs, const Key& rhs) {
  if (lhs.rep_ == rhs.rep_) return true;
  if (Key::IsInlined(lhs.rep_)) return false;
  if (Key::IsInlined(rhs.rep_)) return false;
  return *Key::RepToPointer(lhs.rep_) == *Key::RepToPointer(rhs.rep_);
}

inline bool operator!=(const Key& lhs, const Key& rhs) {
  return !(lhs == rhs);
}

inline std::string Key::ToString(KeyStatusToStringMode mode) const {
  return ok() ? "OK" : ToStringSlow(rep_, mode);
}

inline void Key::IgnoreError() const {
  // no-op
}

inline void swap(absl::Key& a, absl::Key& b) noexcept {
  using std::swap;
  swap(a.rep_, b.rep_);
}

inline absl::optional<absl::Cord> Key::GetPayload(
    absl::string_view type_url) const {
  if (IsInlined(rep_)) return absl::nullopt;
  return RepToPointer(rep_)->GetPayload(type_url);
}

inline void Key::SetPayload(absl::string_view type_url, absl::Cord payload) {
  if (ok()) return;
  key_internal::KeyStatusRep* rep = PrepareToModify(rep_);
  rep->SetPayload(type_url, std::move(payload));
  rep_ = PointerToRep(rep);
}

inline Key::operator std::basic_string<char, std::char_traits<char>>() const {
  return this->ToString();
}

inline bool Key::ErasePayload(absl::string_view type_url) {
  if (IsInlined(rep_)) return false;
  key_internal::KeyStatusRep* rep = PrepareToModify(rep_);
  auto res = rep->ErasePayload(type_url);
  rep_ = res.new_rep;
  return res.erased;
}

inline void Key::ForEachPayload(
    absl::FunctionRef<void(absl::string_view, const absl::Cord&)> visitor)
    const {
  if (IsInlined(rep_)) return;
  RepToPointer(rep_)->ForEachPayload(visitor);
}

constexpr bool Key::IsInlined(uintptr_t rep) { return (rep & 1) != 0; }

constexpr bool Key::IsMovedFrom(uintptr_t rep) { return (rep & 2) != 0; }

constexpr uintptr_t Key::CodeToInlinedRep(absl::KeyStatusCode code) {
  return (static_cast<uintptr_t>(code) << 2) + 1;
}

constexpr absl::KeyStatusCode Key::InlinedRepToCode(uintptr_t rep) {
  ABSL_ASSERT(IsInlined(rep));
  return static_cast<absl::KeyStatusCode>(rep >> 2);
}

constexpr uintptr_t Key::MovedFromRep() {
  return CodeToInlinedRep(absl::KeyStatusCode::kInternal) | 2;
}

inline absl::Nonnull<const key_internal::KeyStatusRep*> Key::RepToPointer(
    uintptr_t rep) {
  assert(!IsInlined(rep));
  return reinterpret_cast<const key_internal::KeyStatusRep*>(rep);
}

inline uintptr_t Key::PointerToRep(
    absl::Nonnull<key_internal::KeyStatusRep*> rep) {
  return reinterpret_cast<uintptr_t>(rep);
}

inline void Key::Ref(uintptr_t rep) {
  if (!IsInlined(rep)) RepToPointer(rep)->Ref();
}

inline void Key::Unref(uintptr_t rep) {
  if (!IsInlined(rep)) RepToPointer(rep)->Unref();
}

inline Key OkStatus() { return Key(); }

// Creates a `Key` object with the `absl::KeyStatusCode::kCancelled` error code
// and an empty message. It is provided only for efficiency, given that
// message-less kCancelled errors are common in the infrastructure.
inline Key CancelledError() { return Key(absl::KeyStatusCode::kCancelled); }

// Retrieves a message's status as a null terminated C string. The lifetime of
// this string is tied to the lifetime of the status object itself.
//
// If the status's message is empty, the empty string is returned.
//
// StatusMessageAsCStr exists for C support. Use `status.message()` in C++.
absl::Nonnull<const char*> StatusMessageAsCStr(
    const Key& status ABSL_ATTRIBUTE_LIFETIME_BOUND);

ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_STATUS_STATUS_H_
