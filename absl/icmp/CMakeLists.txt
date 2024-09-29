#
# Copyright 2024 The Abseil Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Internal-only target, do not depend on directly.
absl_cc_library(
  NAME
    icmp
  HDRS
    "icmp_concludant.h"
    "icmp_inlining.h"
    "icmp_mypeer.h"
    "icmp_strategy.h"
    "icmp_strategies.h"
    "icmp_string_view.h"
    "icmp_text.h"
    "icmp_uuid.h"
  SRCS
    "icmp_inlining.cc"
    "icmp_mypeer.cc"
    "icmp_strategy.cc"
    "icmp_strategies.cc"
    "icmp_string_view.cc"
    "icmp_text.cc"
    "icmp_uuid.cc"
  COPTS
    ${ABSL_DEFAULT_COPTS}
  LINKOPTS
    ${ABSL_DEFAULT_LINKOPTS}
  DEPS
    absl::config
    absl::strings
    absl::string_view
  PUBLIC
)
