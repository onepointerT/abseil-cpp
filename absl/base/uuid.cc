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

#include <random>
#include <sstream>

#include "absl/base/uuid.h"


namespace absl {
ABSL_NAMESPACE_BEGIN



UUID::UUID()
  : absl::string_view( absl::UUID::generate().c_str() )
{}

UUID::UUID( const std::string uuidval )
  : absl::string_view( uuidval.c_str() )
{}


UUID::UUID( const std::basic_string_view<char> uuidval )
  : UUID( std::string(uuidval.data()) )
{}


UUID::operator std::basic_string<char, std::char_traits<char>, std::allocator<char>>() const {
  return std::string( this->data() );
}

const std::string UUID::generate() {
  std::stringstream s;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 15);
  std::uniform_int_distribution<> dis2(8, 11);

  s << std::hex;
  for (unsigned int i = 0; i < 8; i++) {
      s << dis(gen);
  }
  s << "-";
  for (unsigned int i = 0; i < 4; i++) {
      s << dis(gen);
  }
  s << "-4";
  for (unsigned int i = 0; i < 3; i++) {
      s << dis(gen);
  }
  s << "-";
  s << dis2(gen);
  for (unsigned int i = 0; i < 3; i++) {
      s << dis(gen);
  }
  s << "-";
  for (unsigned int i = 0; i < 12; i++) {
      s << dis(gen);
  }
  return s.str();
}



ABSL_NAMESPACE_END
} // namespace absl

