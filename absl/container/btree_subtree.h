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
// File: btree_subtree.h
// -----------------------------------------------------------------------------
//
// This header file defines B-tree sets: sorted associative containers of
// values.
//
//     * `absl::btree_set<>`
//     * `absl::btree_subtree<>`
//     * `absl::btree_set< absl::KeyValuePair< absl::string_view, absl::btree_subtree<> > >`
//
// These B-tree types are similar to the corresponding types in the STL
// (`std::set` and `std::multiset`) and generally conform to the STL interfaces
// of those types. However, because they are implemented using B-trees, they
// are more efficient in most situations.
//
// Unlike `std::set` and `std::multiset`, which are commonly implemented using
// red-black tree nodes, B-tree sets use more generic B-tree nodes able to hold
// multiple values per node. Holding multiple values per node often makes
// B-tree sets perform better than their `std::set` counterparts, because
// multiple entries can be checked within the same cache hit.
//
// However, these types should not be considered drop-in replacements for
// `std::set` and `std::multiset` as there are some API differences, which are
// noted in this header file. The most consequential differences with respect to
// migrating to b-tree from the STL types are listed in the next paragraph.
// Other API differences are minor.
//
// Importantly, insertions and deletions may invalidate outstanding iterators,
// pointers, and references to elements. Such invalidations are typically only
// an issue if insertion and deletion operations are interleaved with the use of
// more than one iterator, pointer, or reference simultaneously. For this
// reason, `insert()`, `erase()`, and `extract_and_get_next()` return a valid
// iterator at the current position.
//
// Another API difference is that btree iterators can be subtracted, and this
// is faster than using std::distance.
//
// B-tree sets are not exception-safe.

#ifndef ABSL_CONTAINER_BTREE_SUBTREE_H_
#define ABSL_CONTAINER_BTREE_SUBTREE_H_

#include "absl/base/attributes.h"
#include "absl/container/btree_set.h"
#include "absl/container/key.h"
#include "absl/strings/string_view.h"

namespace absl {
ABSL_NAMESPACE_BEGIN


template< typename KeyT, typename ValueT >
using btree_subtree = typename absl::btree_set< absl::KeyValuePair< KeyT, ValueT > >;

template< typename KeyT, typename ValueT >
class Subtree
  : public btree_subtree< KeyT, ValueT >
{
  public:
    using btree_iterator = btree_subtree< KeyT, ValueT >::iterator;
    using subtree_iterator = Subtree::iterator;

    template< typename Tree >
    class SubtreeIterator
      : public absl::container_internal::btree_container< Tree >::TreeIterator
    {
      public:
        typedef typename absl::container_internal::btree_container< Tree > Base;
        
        subtree_iterator subtree_pos = current_pos->second->begin();
        btree_iterator current_pos;

        using Base::next;
        bool next_in_subtree();
    };
};


template< typename ValueT >
class btree_subtree_alphabetical
  : public absl::btree_subtree< absl::string_view, ValueT >
{
 public:
  typedef typename absl::KeyValuePair< absl::string_view, ValueT > key_t;
  typedef typename std::span< ValueT >::type value_t;
};



ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_CONTAINER_BTREE_SUBTREE_H_
