// include/beman/net29/detail/execution.hpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_NET29_DETAIL_EXECUTION
#define INCLUDED_INCLUDE_BEMAN_NET29_DETAIL_EXECUTION

#include <beman/execution26/execution.hpp>

// ----------------------------------------------------------------------------

namespace beman::net29::detail::ex
{
    using ::beman::execution26::completion_signatures;

    using ::beman::execution26::get_env;
    using ::beman::execution26::empty_env;
    using ::beman::execution26::value_types_of_t;
    using ::beman::execution26::error_types_of_t;

    using ::beman::execution26::get_stop_token_t;
    using ::beman::execution26::get_stop_token;
    using ::beman::execution26::get_completion_signatures_t;
    using ::beman::execution26::get_completion_signatures;
    using ::beman::execution26::get_completion_scheduler_t;
    using ::beman::execution26::get_completion_scheduler;
    using ::beman::execution26::get_delegation_scheduler_t;
    using ::beman::execution26::get_delegation_scheduler;
    using ::beman::execution26::get_scheduler_t;
    using ::beman::execution26::get_scheduler;

    using ::beman::execution26::operation_state_t;
    using ::beman::execution26::operation_state;
    using ::beman::execution26::receiver_t;
    using ::beman::execution26::receiver;
    using ::beman::execution26::sender_t;
    using ::beman::execution26::sender;
    using ::beman::execution26::scheduler_t;
    using ::beman::execution26::scheduler;

    using ::beman::execution26::sender_in;

    using ::beman::execution26::set_error_t;
    using ::beman::execution26::set_error;
    using ::beman::execution26::set_stopped_t;
    using ::beman::execution26::set_stopped;
    using ::beman::execution26::set_value_t;
    using ::beman::execution26::set_value;

    using ::beman::execution26::connect_t;
    using ::beman::execution26::connect;
    using ::beman::execution26::start_t;
    using ::beman::execution26::start;

    using ::beman::execution26::detail::write_env;
    using ::beman::execution26::just;
    using ::beman::execution26::just_error;
    using ::beman::execution26::just_stopped;
    using ::beman::execution26::then;
    using ::beman::execution26::upon_error;
    using ::beman::execution26::upon_stopped;
    using ::beman::execution26::sync_wait;
}

// ----------------------------------------------------------------------------

#endif
