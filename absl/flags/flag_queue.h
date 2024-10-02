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

#ifndef ABSL_FLAGS_FLAGQUEUE_H_
#define ABSL_FLAGS_FLAGQUEUE_H_

#include <vector>


namespace absl {
ABSL_NAMESPACE_BEGIN



template< typename flags_t >
class FlagQueue 
    :   public std::vector< flags_t* >
{
public:
    FlagQueue()
        :   std::vector< flags_t* >()
    {}

    // Push a flags_t object inside the queue
    virtual void push( flags_t* flags ) {
        this->emplace_back( flags );
    }

    // Get the most top flags_t object from the queue
    flags_t* pop() {
        flags_t* elem = this->at(0);
        this->erase(this->begin());
        return elem;
    }
};


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_FLAGS_FLAGQUEUE_H_
