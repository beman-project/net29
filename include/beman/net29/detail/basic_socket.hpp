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

template <typename _Protocol>
class beman::net29::basic_socket
    : public ::beman::net29::socket_base
{
public:
    using scheduler_type     = ::beman::net29::detail::io_context_scheduler;
    using protocol_type      = _Protocol;

private:
    static constexpr ::beman::net29::detail::socket_id _S_unused{0xffff'ffff};
    ::beman::net29::detail::context_base* _D_context;
    protocol_type                     _D_protocol{::beman::net29::ip::tcp::v6()}; 
    ::beman::net29::detail::socket_id     _D_id{_S_unused};

public:
    basic_socket()
        : _D_context(nullptr)
    {
    }
    basic_socket(::beman::net29::detail::context_base* context, ::beman::net29::detail::socket_id _Id)
        : _D_context(context)
        , _D_id(_Id)
    {
    }
    basic_socket(basic_socket&& _Other)
        : _D_context(_Other._D_context)
        , _D_protocol(_Other._D_protocol)
        , _D_id(::std::exchange(_Other._D_id, _S_unused))
    {
    }
    ~basic_socket()
    {
        if (this->_D_id != _S_unused)
        {
            ::std::error_code _Error{};
            this->_D_context->_Release(this->_D_id, _Error);
        }
    }
    auto get_scheduler() noexcept -> scheduler_type
    {
        return scheduler_type{this->_D_context};
    }
    auto _Id() const -> ::beman::net29::detail::socket_id { return this->_D_id; }
};


// ----------------------------------------------------------------------------

#endif
