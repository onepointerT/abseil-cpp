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
// This header file contains all classes the plugin compositor and the
// plugin context need for the plugin base and the plugin-api-base.
// Please inherit from those classes here.
//

#ifndef ABSL_ICMP_ICMP_UUID_H_
#define ABSL_ICMP_ICMP_UUID_H_

#include <string>
#include <utility>

#include "absl/base/config.h"
#include "absl/base/uuid.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


class IcmpUUID final
    :   public absl::UUID
{
protected:
    absl::string_view m_last_instertion;

    // Returns the next UUID gap value
    //  of inserter_text, if any and m_last_insertion set
    const std::basic_string<char> next_uuid() const noexcept;
public:
    const std::string inserter_text;
    std::string replacement_text;

    IcmpUUID( const std::basic_string<char> template_text
            , const UUID other = std::string()
    );

    // Returns tuple ((inserter_pos,  inserter_count_until_pos)
    std::pair<std::pair<size_t, size_t>, const unsigned int> find_inserter_end( const UUID uuid = std::string() ) const;
    // Returns tuple (inserter_start_pos, inserter_end_pos)
    std::pair<size_t, size_t> find_inserter_pos( const UUID uuid = std::string() ) const;

    // Assembles the next gap's IcmpUUID, if any.
    IcmpUUID* on_next_inserter_start( const std::string inserter_string );

    unsigned int count_subuuids();

    bool merge_into( const UUID& gapid, const std::basic_string<char>& text );
    bool merge_into( const std::basic_string<char> text );
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_ICMP_ICMP_UUID_H_
