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

#include "absl/plugin/compositor.h"



namespace absl {


PluginBase::PluginBase()
    :   std::map< const absl::string_view, PluginBase* >()
{}


std::pair< bool, PluginBase* > PluginBase::get_plugin( const std::string name ) const {
    bool is_there = this->contains(name);
    return std::make_pair(is_there, is_there ? this->at(name) : nullptr );
}


bool PluginBase::add_plugin( const std::string plugin_name, PluginBase* plugin ) {
    (*this)[plugin_name] = plugin;
    return true;
}


} // namespace absl
