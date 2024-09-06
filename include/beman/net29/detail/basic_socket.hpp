// include/beman/net29/detail/basic_socket.hpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_BASIC_SOCKET
#define INCLUDED_BEMAN_NET29_DETAIL_BASIC_SOCKET

// ----------------------------------------------------------------------------

#include <beman/net29/detail/netfwd.hpp>
#include <beman/net29/detail/socket_base.hpp>
#include <beman/net29/detail/io_context_scheduler.hpp>
#include <beman/net29/detail/internet.hpp>

// ----------------------------------------------------------------------------

template <typename Protocol>
class beman::net29::basic_socket
    : public ::beman::net29::socket_base
{
public:
    using scheduler_type     = ::beman::net29::detail::io_context_scheduler;
    using protocol_type      = Protocol;

private:
    ::beman::net29::detail::context_base* d_context;
    protocol_type                     d_protocol{::beman::net29::ip::tcp::v6()}; 
    ::beman::net29::detail::socket_id     d_id{::beman::net29::detail::socket_id::invalid};

public:
    basic_socket()
        : d_context(nullptr)
    {
    }
    basic_socket(::beman::net29::detail::context_base* context, ::beman::net29::detail::socket_id id)
        : d_context(context)
        , d_id(id)
    {
    }
    basic_socket(basic_socket&& other)
        : d_context(other.d_context)
        , d_protocol(other.d_protocol)
        , d_id(::std::exchange(other.d_id, ::beman::net29::detail::socket_id::invalid))
    {
    }
    ~basic_socket()
    {
        if (this->d_id != ::beman::net29::detail::socket_id::invalid)
        {
            ::std::error_code error{};
            this->d_context->release(this->d_id, error);
        }
    }
    auto get_scheduler() noexcept -> scheduler_type
    {
        return scheduler_type{this->d_context};
    }
    auto id() const -> ::beman::net29::detail::socket_id { return this->d_id; }
};


// ----------------------------------------------------------------------------

#endif
