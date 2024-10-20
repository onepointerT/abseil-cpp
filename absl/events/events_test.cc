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

#include "absl/events/events.h"




namespace absl::test {


class C1
    :   public EventObject<C1>
{
public:
    bool start_an_event_s1();
};


class C2
    :   public EventObject<C2>
{
protected:
    bool receive_an_event_c2();

    friend class C1;
public:
    C2( C1* c1_instance ) {
        absl_connect("c2_c1", c1_instance->start_an_event_s1, this->receive_an_event_c2, this)
    }

    bool start_an_event_c2() {
        this->emit_func("c2_c1", false)
    }
};


} // namespace absl


int main(int argc, char* []argv) {

    absl::test::C1* c1 = new absl::test::C1();
    absl::test::C2* c2 = new absl::test::C2(c1);

    return 0;
}
