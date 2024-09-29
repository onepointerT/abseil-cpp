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
// This header file defines an class and the algorithm for declaring UUID values.
//
// 
//

#ifndef ABSL_BASE_UUID_H_
#define ABSL_BASE_UUID_H_

#include <string>

#include "absl/base/config.h"
#include "absl/strings/string_view.h"


namespace absl {
ABSL_NAMESPACE_BEGIN


class UUID
  : public absl::string_view
{
 public:
  UUID();
  UUID( const std::string uuidval );
  UUID( const std::basic_string_view<char> uuidval );

  operator std::basic_string<char>() const;

  static const std::string generate();
};


ABSL_NAMESPACE_END
} // namespace absl


#endif  // ABSL_BASE_UUID_H_
