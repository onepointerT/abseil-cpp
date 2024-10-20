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

#ifndef ABSL_EVENTS_EVENTMAP_H_
#define ABSL_EVENTS_EVENTMAP_H_

#include <future>
#include <initializer_list>
#include <map>
#include <string>
#include <utility>
#include <variant>

#include "absl/base/config.h"
#include "absl/events/eventqueue.h"
#include "absl/functional/function_arguments.h"
#include "absl/functional/function_ref.h"

namespace absl {
ABSL_NAMESPACE_BEGIN

template< class R >
class EventConnector;

template< typename R = bool >
using func_t = FunctionRef<R>;
template< typename R, typename ...Args >
using funcref_t = FunctionRef<R(Args...)>;
template< typename ...Args >
using funcref_type = funcref_t<bool, Args...>;


template< class base_cls_t >
class EventObject
{
private:


protected:
    typedef base_cls_t base_t;
    
    template< typename clsptr, typename ...Args >
    void ref( std::string signame, funcref_type<Args...> signal, funcref_type<Args...> receiver, clsptr* receiving_instance ) {
        EventMap<>::ref(signame, signal, receiver, receiving_instance);
    }

    template< typename il_t, typename ...Args >
    std::initializer_list<il_t>& emit_func( const std::string signame, Args&&... args ) {
        std::future<bool> emitsig = std::async( std::launch::async, EventMap<>::emit( this, signame, args ) );
        emitsig.wait();
        return EventMap<>::results( signame );
    }

    friend class EventConnector<bool>;
};


template< class R >
class EventMap;

template< class R >
class EventConnector
    :   protected std::pair< FunctionRef<R>, FunctionRef<R> >
{
protected:
    typedef R return_t;

    template< typename ...Types >
    using variant_t = typename std::variant< Types... >;

    std::vector< std::identity >* argument_list;

    template< class receiver_t, typename ...Args >
    EventConnector( FunctionRef<R(Args...)>& signal
                  , FunctionRef<R(Args...)>& receiver
                  , receiver_t* receiving_instance
    )
        :   std::pair< func_t, func_t >( signal, receiver )
        ,   argument_list( new std::vector< std::identity<std::variant<Args...>> >() )
        ,   recv_instance( receiving_instance )
    {}

    friend class EventMap<R>;

public:
    template< typename clsptr, typename ...Args >
    bool invoke( clsptr* cls_ptr, Args... args ) {
        std::future<bool> signal = this->first( args );
        if ( signal.get() ) {
            std::future<bool> receiver = std::async( std::launch::async, this->second( args ) );
            if ( receiver.get() ) {
                this->set_variant(args);
                recv_instance->notify( args );
            }
        }
        return false;
    }

    template< typename clsptr >
    bool invoke( clsptr* cls_ptr ) {
        return this->invoke( cls_ptr, this->argument_list );
    }

    template< typename il_t, typename ...Types >
    void set_variant( std::initializer_list<il_t> il ) {
        this->argument_list.emplace( il );
    }

    template< typename il_t, typename ...Types >
    void set_variant( Types... args ) {
        std::initializer_list<il_t> il{args}
        this->argument_list.emplace( il );
    }
};



template< typename R = bool >
class EventMap
{
protected:
    static std::map< std::string, EventConnector<R>* > eventconn;

    EventMap() = delete;

public:
    template< typename clsptr, typename ...Args >
    static void ref( std::string signame, funcref_type<Args...>& signal, funcref_type<Args...>& receiver, clsptr* receiving_instance ) {
        EventConnector<R>* ec = new EventConnector<Args...>(signal, receiver, receiving_instance);
        this->eventconn[signame] = ec;
    }

    template< typename ...Types >
    static std::variant<Types...> results( const std::string signame ) {
        if ( ! this->contains( signame ) ) {
            return std::variant<Types...>();
        }

        EventConnector<R>* ec = this->at( signame );
        return ec->argument_list;
    }

    template< typename clsptr, typename il_t >
    static bool emit( clsptr* cls_ptr, const std::string signame, std::initializer_list<il_t> il ) {
        if ( ! this->contains( signame ) ) {
            return false;
        }

        EventConnector<R>* ec = this->at( signame );
        ec->set_variant( il );
        return ec->invoke<>( cls_ptr );
    }

    template< typename il_t, typename ...Args >
    static bool emit( const std::string signame, Args&&... args ) {
        std::initializer_list<il_t> il{args};
        return this->emit( signame, il );
    }
};


#define absl_connect(signame, signal, receiver, recv_inst) EventMap<>::ref(signame, signal, receiver, recv_inst);
#define absl_emit(signame) EventMap<>::emit(signame);

ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_EVENTS_EVENTS_H_
