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
#include "absl/container/key.h"

#include <errno.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <ostream>
#include <string>

#include "absl/base/attributes.h"
#include "absl/base/config.h"
#include "absl/base/internal/raw_logging.h"
#include "absl/base/internal/strerror.h"
#include "absl/base/macros.h"
#include "absl/base/no_destructor.h"
#include "absl/base/nullability.h"
#include "absl/container/internal/key_internal.h"
#include "absl/debugging/stacktrace.h"
#include "absl/debugging/symbolize.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "absl/types/optional.h"

namespace absl {
ABSL_NAMESPACE_BEGIN

static_assert(
    alignof(key_internal::KeyStatusRep) >= 4,
    "absl::KeyStatus assumes it can use the bottom 2 bits of a StatusRep*.");

std::string StatusCodeToString(KeyStatusCode code) {
  switch (code) {
    case KeyStatusCode::kOk:
      return "OK";
    case KeyStatusCode::kCancelled:
      return "CANCELLED";
    case KeyStatusCode::kUnknown:
      return "UNKNOWN";
    case KeyStatusCode::kInvalidArgument:
      return "INVALID_ARGUMENT";
    case KeyStatusCode::kDeadlineExceeded:
      return "DEADLINE_EXCEEDED";
    case KeyStatusCode::kNotFound:
      return "NOT_FOUND";
    case KeyStatusCode::kAlreadyExists:
      return "ALREADY_EXISTS";
    case KeyStatusCode::kPermissionDenied:
      return "PERMISSION_DENIED";
    case KeyStatusCode::kUnauthenticated:
      return "UNAUTHENTICATED";
    case KeyStatusCode::kResourceExhausted:
      return "RESOURCE_EXHAUSTED";
    case KeyStatusCode::kFailedPrecondition:
      return "FAILED_PRECONDITION";
    case KeyStatusCode::kAborted:
      return "ABORTED";
    case KeyStatusCode::kOutOfRange:
      return "OUT_OF_RANGE";
    case KeyStatusCode::kUnimplemented:
      return "UNIMPLEMENTED";
    case KeyStatusCode::kInternal:
      return "INTERNAL";
    case KeyStatusCode::kUnavailable:
      return "UNAVAILABLE";
    case KeyStatusCode::kDataLoss:
      return "DATA_LOSS";
    case KeyStatusCode::kChanged:
      return "CHANGED";
    case KeyStatusCode::kValueUntouched:
      return "VALUE_UNTOUCHED";
    case KeyStatusCode::kValueForKeyAvailable:
      return "VALUE_FOR_KEY_AVAILABLE";
    default:
      return "";
  }
}

std::ostream& operator<<(std::ostream& os, KeyStatusCode code) {
  return os << StatusCodeToString(code);
}

absl::Nonnull<const std::string*> Key::EmptyString() {
  static const absl::NoDestructor<std::string> kEmpty;
  return kEmpty.get();
}

#ifdef ABSL_INTERNAL_NEED_REDUNDANT_CONSTEXPR_DECL
constexpr const char KeyStatus::kMovedFromString[];
#endif

absl::Nonnull<const std::string*> Key::MovedFromString() {
  static const absl::NoDestructor<std::string> kMovedFrom(kMovedFromString);
  return kMovedFrom.get();
}

Key::Key(absl::KeyStatusCode code, absl::string_view msg)
    : rep_(Key::CodeToInlinedRep(code)) {
  if (code != absl::KeyStatusCode::kOk && !msg.empty()) {
    rep_ = Key::PointerToRep(new key_internal::KeyStatusRep(code, msg, nullptr));
  }
}

absl::Nonnull<key_internal::KeyStatusRep*> Key::PrepareToModify(
    uintptr_t rep) {
  if (IsInlined(rep)) {
    return new key_internal::KeyStatusRep(InlinedRepToCode(rep),
                                          absl::string_view(), nullptr);
  }
  return RepToPointer(rep)->CloneAndUnref();
}

std::string Key::ToStringSlow(uintptr_t rep, absl::KeyStatusToStringMode mode) {
  if (IsInlined(rep)) {
    return absl::StrCat(absl::StatusCodeToString(InlinedRepToCode(rep)), ": ");
  }
  return RepToPointer(rep)->ToString(mode);
}

std::ostream& operator<<(std::ostream& os, const KeyStatus& x) {
  os << x.ToString(KeyStatusToStringMode::kWithEverything);
  return os;
}

KeyStatus AbortedError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kAborted, message);
}

KeyStatus AlreadyExistsError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kAlreadyExists, message);
}

KeyStatus CancelledError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kCancelled, message);
}

KeyStatus DataLossError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kDataLoss, message);
}

KeyStatus DeadlineExceededError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kDeadlineExceeded, message);
}

KeyStatus FailedPreconditionError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kFailedPrecondition, message);
}

KeyStatus InternalError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kInternal, message);
}

KeyStatus InvalidArgumentError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kInvalidArgument, message);
}

KeyStatus NotFoundError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kNotFound, message);
}

KeyStatus OutOfRangeError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kOutOfRange, message);
}

KeyStatus PermissionDeniedError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kPermissionDenied, message);
}

KeyStatus ResourceExhaustedError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kResourceExhausted, message);
}

KeyStatus UnauthenticatedError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kUnauthenticated, message);
}

KeyStatus UnavailableError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kUnavailable, message);
}

KeyStatus UnimplementedError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kUnimplemented, message);
}

KeyStatus UnknownError(absl::string_view message) {
  return KeyStatus(absl::KeyStatusCode::kUnknown, message);
}

bool IsAborted(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kAborted;
}

bool IsAlreadyExists(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kAlreadyExists;
}

bool IsCancelled(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kCancelled;
}

bool IsDataLoss(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kDataLoss;
}

bool IsDeadlineExceeded(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kDeadlineExceeded;
}

bool IsFailedPrecondition(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kFailedPrecondition;
}

bool IsInternal(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kInternal;
}

bool IsInvalidArgument(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kInvalidArgument;
}

bool IsNotFound(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kNotFound;
}

bool IsOutOfRange(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kOutOfRange;
}

bool IsPermissionDenied(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kPermissionDenied;
}

bool IsResourceExhausted(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kResourceExhausted;
}

bool IsUnauthenticated(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kUnauthenticated;
}

bool IsUnavailable(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kUnavailable;
}

bool IsUnimplemented(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kUnimplemented;
}

bool IsUnknown(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kUnknown;
}

bool IsChanged(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kChanged;
}

bool IsValueUntouched(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kValueUntouched;
}

bool IsValueForKeyAvailable(const KeyStatus& status) {
  return status.code() == absl::KeyStatusCode::kValueForKeyAvailable;
}

KeyStatusCode ErrnoToStatusCode(int error_number) {
  switch (error_number) {
    case 0:
      return KeyStatusCode::kOk;
    case EINVAL:        // Invalid argument
    case ENAMETOOLONG:  // Filename too long
    case E2BIG:         // Argument list too long
    case EDESTADDRREQ:  // Destination address required
    case EDOM:          // Mathematics argument out of domain of function
    case EFAULT:        // Bad address
    case EILSEQ:        // Illegal byte sequence
    case ENOPROTOOPT:   // Protocol not available
    case ENOTSOCK:      // Not a socket
    case ENOTTY:        // Inappropriate I/O control operation
    case EPROTOTYPE:    // Protocol wrong type for socket
    case ESPIPE:        // Invalid seek
      return KeyStatusCode::kInvalidArgument;
    case ETIMEDOUT:  // Connection timed out
      return KeyStatusCode::kDeadlineExceeded;
    case ENODEV:  // No such device
    case ENOENT:  // No such file or directory
#ifdef ENOMEDIUM
    case ENOMEDIUM:  // No medium found
#endif
    case ENXIO:  // No such device or address
    case ESRCH:  // No such process
      return KeyStatusCode::kNotFound;
    case EEXIST:         // File exists
    case EADDRNOTAVAIL:  // Address not available
    case EALREADY:       // Connection already in progress
#ifdef ENOTUNIQ
    case ENOTUNIQ:  // Name not unique on network
#endif
      return KeyStatusCode::kAlreadyExists;
    case EPERM:   // Operation not permitted
    case EACCES:  // Permission denied
#ifdef ENOKEY
    case ENOKEY:  // Required key not available
#endif
    case EROFS:  // Read only file system
      return KeyStatusCode::kPermissionDenied;
    case ENOTEMPTY:   // Directory not empty
    case EISDIR:      // Is a directory
    case ENOTDIR:     // Not a directory
    case EADDRINUSE:  // Address already in use
    case EBADF:       // Invalid file descriptor
#ifdef EBADFD
    case EBADFD:  // File descriptor in bad state
#endif
    case EBUSY:    // Device or resource busy
    case ECHILD:   // No child processes
    case EISCONN:  // Socket is connected
#ifdef EISNAM
    case EISNAM:  // Is a named type file
#endif
#ifdef ENOTBLK
    case ENOTBLK:  // Block device required
#endif
    case ENOTCONN:  // The socket is not connected
    case EPIPE:     // Broken pipe
#ifdef ESHUTDOWN
    case ESHUTDOWN:  // Cannot send after transport endpoint shutdown
#endif
    case ETXTBSY:  // Text file busy
#ifdef EUNATCH
    case EUNATCH:  // Protocol driver not attached
#endif
      return KeyStatusCode::kFailedPrecondition;
    case ENOSPC:  // No space left on device
#ifdef EDQUOT
    case EDQUOT:  // Disk quota exceeded
#endif
    case EMFILE:   // Too many open files
    case EMLINK:   // Too many links
    case ENFILE:   // Too many open files in system
    case ENOBUFS:  // No buffer space available
    case ENOMEM:   // Not enough space
#ifdef EUSERS
    case EUSERS:  // Too many users
#endif
      return KeyStatusCode::kResourceExhausted;
#ifdef ECHRNG
    case ECHRNG:  // Channel number out of range
#endif
    case EFBIG:      // File too large
    case EOVERFLOW:  // Value too large to be stored in data type
    case ERANGE:     // Result too large
      return KeyStatusCode::kOutOfRange;
#ifdef ENOPKG
    case ENOPKG:  // Package not installed
#endif
    case ENOSYS:        // Function not implemented
    case ENOTSUP:       // Operation not supported
    case EAFNOSUPPORT:  // Address family not supported
#ifdef EPFNOSUPPORT
    case EPFNOSUPPORT:  // Protocol family not supported
#endif
    case EPROTONOSUPPORT:  // Protocol not supported
#ifdef ESOCKTNOSUPPORT
    case ESOCKTNOSUPPORT:  // Socket type not supported
#endif
    case EXDEV:  // Improper link
      return KeyStatusCode::kUnimplemented;
    case EAGAIN:  // Resource temporarily unavailable
#ifdef ECOMM
    case ECOMM:  // Communication error on send
#endif
    case ECONNREFUSED:  // Connection refused
    case ECONNABORTED:  // Connection aborted
    case ECONNRESET:    // Connection reset
    case EINTR:         // Interrupted function call
#ifdef EHOSTDOWN
    case EHOSTDOWN:  // Host is down
#endif
    case EHOSTUNREACH:  // Host is unreachable
    case ENETDOWN:      // Network is down
    case ENETRESET:     // Connection aborted by network
    case ENETUNREACH:   // Network unreachable
    case ENOLCK:        // No locks available
    case ENOLINK:       // Link has been severed
#ifdef ENONET
    case ENONET:  // Machine is not on the network
#endif
      return KeyStatusCode::kUnavailable;
    case EDEADLK:  // Resource deadlock avoided
#ifdef ESTALE
    case ESTALE:  // Stale file handle
#endif
      return KeyStatusCode::kAborted;
    case ECANCELED:  // Operation cancelled
      return KeyStatusCode::kCancelled;
    default:
      return KeyStatusCode::kUnknown;
  }
}

namespace {
std::string MessageForErrnoToStatus(int error_number,
                                    absl::string_view message) {
  return absl::StrCat(message, ": ",
                      absl::base_internal::StrError(error_number));
}
}  // namespace

KeyStatus ErrnoToStatus(int error_number, absl::string_view message) {
  return KeyStatus(ErrnoToStatusCode(error_number),
                MessageForErrnoToStatus(error_number, message));
}

absl::Nonnull<const char*> StatusMessageAsCStr(const KeyStatus& status) {
  // As an internal implementation detail, we guarantee that if status.message()
  // is non-empty, then the resulting string_view is null terminated.
  const absl::string_view sv_message = status.message();
  return sv_message.empty() ? "" : sv_message.data();
}

ABSL_NAMESPACE_END
}  // namespace absl
