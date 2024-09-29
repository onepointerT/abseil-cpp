// Copyright 2024 The Abseil Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -----------------------------------------------------------------------------
// File: singleton.h
// -----------------------------------------------------------------------------
//
// This header file defines prefetch functions to prefetch memory contents
// into the first level cache (L1) for the current CPU. The prefetch logic
// offered in this header is limited to prefetching first level cachelines
// only, and is aimed at relatively 'simple' prefetching logic.
//
#ifndef ABSL_BASE_SINGLETON_H_
#define ABSL_BASE_SINGLETON_H_

#include <memory>

#include "absl/base/config.h"



namespace absl {
ABSL_NAMESPACE_BEGIN


template< typename inheriting_t >
class Singleton {
protected:
    // You need to initialize the static pointer inline or in-compile-file.
    static std::unique_ptr< inheriting_t > ms_instance;
    
    explicit Singleton() {
        Singleton< inheriting_t >::ms_instance
            = std::unique_ptr< inheriting_t >( nullptr );
    }
    virtual ~Singleton();

public:
    typedef inheriting_t inheriting_type;

    static inheriting_type* get() {
        if ( ! ms_instance ) {
            ms_instance = std::unique_ptr< inheriting_type >( new inheriting_type() );
        }
        return ms_instance.get();
    }
};


template< class inheriting_t >
inline std::unique_ptr< inheriting_t > Singleton<inheriting_t>::ms_instance
        = std::unique_ptr< inheriting_t >( nullptr );


ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_BASE_SINGLETON_H_
