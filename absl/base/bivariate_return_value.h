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

#ifndef ABSL_BASE_LOG_SEVERITY_H_
#define ABSL_BASE_LOG_SEVERITY_H_

#include <tuple>
#include <type_traits>

#include "absl/base/attributes.h"
#include "absl/base/config.h"

namespace absl {
ABSL_NAMESPACE_BEGIN

// absl::BivariateReturn
//
// We use conditional with invoke result for Bivariate return values.
//
// Intentionally, if the function is_which returns true, the return type 1
// will be returned and is also the type of the Bivariate return
template< typename R1, typename R2, bool(*is_which)(R1, R2), typename... Args >
struct BivariateReturn
    :   public std::conditional< std::invoke_result<is_which, R1, R2 >(), R1, R2 >::type
{};



ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_BASE_LOG_SEVERITY_H_
