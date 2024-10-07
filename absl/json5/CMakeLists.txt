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

absl_cc_library(
  NAME
    json5
  HDRS
    "json5_base.h"
    "json5_builder.h"
    "json5_input.h"
    "json5_output.h"
    "json5_reflect.h"
    "json5_streams.h"
    "json5.h"
    "json6vars.h"
  SRCS
    "json5_builder.cc"
    "json5_input.cc"
    "json5_output.cc"
    "json5_reflect.cc"
    "json5_streams.cc"
    "json5.cc"
    "json6vars.cc"
  COPTS
    ${ABSL_DEFAULT_COPTS} -std=c++23
  DEPS
    absl::config
  PUBLIC
)
