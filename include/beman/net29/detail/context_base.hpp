// include/beman/net29/detail/context_base.hpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_CONTEXT_BASE
#define INCLUDED_BEMAN_NET29_DETAIL_CONTEXT_BASE

#include <beman/net29/detail/io_base.hpp>
#include <beman/net29/detail/endpoint.hpp>
#include <chrono>
#include <optional>
#include <system_error>
#include <sys/socket.h>
#include <sys/time.h>

// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    struct context_base;
}

// ----------------------------------------------------------------------------

struct beman::net29::detail::context_base
{
    using _Accept_operation = ::beman::net29::detail::io_operation<
        ::std::tuple<::beman::net29::detail::endpoint,
                     ::socklen_t,
                     ::std::optional<::beman::net29::detail::socket_id>
                     >
        >;
    using _Connect_operation = ::beman::net29::detail::io_operation<
        ::std::tuple<::beman::net29::detail::endpoint>
        >;
    using _Receive_operation = ::beman::net29::detail::io_operation<
        ::std::tuple<::msghdr, int, ::std::size_t>
        >;
    using _Send_operation = ::beman::net29::detail::io_operation<
        ::std::tuple<::msghdr, int, ::std::size_t>
        >;
    using _Resume_after_operation = ::beman::net29::detail::io_operation<
        ::std::tuple<::std::chrono::microseconds, ::timeval>
        >;
    using _Resume_at_operation = ::beman::net29::detail::io_operation<
        ::std::tuple<::std::chrono::system_clock::time_point, ::timeval>
        >;

    virtual ~context_base() = default;
    virtual auto _Make_socket(int) -> ::beman::net29::detail::socket_id = 0;
    virtual auto _Make_socket(int, int, int, ::std::error_code&) -> ::beman::net29::detail::socket_id = 0;
    virtual auto _Release(::beman::net29::detail::socket_id, ::std::error_code&) -> void = 0;
    virtual auto _Native_handle(::beman::net29::detail::socket_id) -> ::beman::net29::detail::native_handle_type = 0;
    virtual auto _Set_option(::beman::net29::detail::socket_id, int, int, void const*, ::socklen_t, ::std::error_code&) -> void = 0;
    virtual auto _Bind(::beman::net29::detail::socket_id, ::beman::net29::detail::endpoint const&, ::std::error_code&) -> void = 0;
    virtual auto _Listen(::beman::net29::detail::socket_id, int, ::std::error_code&) -> void = 0;

    virtual auto run_one() -> ::std::size_t = 0;

    virtual auto _Cancel(::beman::net29::detail::io_base*, ::beman::net29::detail::io_base*) -> void = 0;
    virtual auto _Accept(::beman::net29::detail::context_base::_Accept_operation*) -> bool = 0;
    virtual auto _Connect(::beman::net29::detail::context_base::_Connect_operation*) -> bool = 0;
    virtual auto _Receive(::beman::net29::detail::context_base::_Receive_operation*) -> bool = 0;
    virtual auto _Send(::beman::net29::detail::context_base::_Send_operation*) -> bool = 0;
    virtual auto _Resume_after(::beman::net29::detail::context_base::_Resume_after_operation*) -> bool = 0;
    virtual auto _Resume_at(::beman::net29::detail::context_base::_Resume_at_operation*) -> bool = 0;
};

// ----------------------------------------------------------------------------

#endif
