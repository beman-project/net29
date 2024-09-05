// include/beman/net29/detail/basic_stream_socket.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_BASIC_STREAM_SOCKET
#define INCLUDED_BEMAN_NET29_DETAIL_BASIC_STREAM_SOCKET

// ----------------------------------------------------------------------------

#include <beman/net29/detail/netfwd.hpp>
#include <beman/net29/detail/io_context.hpp>
#include <beman/net29/detail/basic_socket.hpp>
#include <functional>
#include <system_error>

// ----------------------------------------------------------------------------

template <typename _Protocol>
class beman::net29::basic_stream_socket
    : public basic_socket<_Protocol>
{
public:
    using native_handle_type = ::beman::net29::detail::native_handle_type;
    using protocol_type = _Protocol;
    using endpoint_type = typename protocol_type::endpoint;

private:
    endpoint_type _D_endpoint;

public:
    basic_stream_socket(basic_stream_socket&&) = default;
    basic_stream_socket& operator= (basic_stream_socket&&) = default;
    basic_stream_socket(::beman::net29::detail::context_base* context, ::beman::net29::detail::socket_id _Id)
        : basic_socket<_Protocol>(context, _Id)
    {
    }
    basic_stream_socket(::beman::net29::io_context& context, endpoint_type const& _Endpoint)
        : beman::net29::basic_socket<_Protocol>(context.get_scheduler()._Get_context(),
            ::std::invoke([_P = _Endpoint.protocol(), &context]{
                ::std::error_code _Error{};
                auto _Rc(context._Make_socket(_P.family(), _P.type(), _P.protocol(), _Error));
                if (_Error)
                {
                    throw ::std::system_error(_Error);
                }
                return _Rc;
            }))
        , _D_endpoint(_Endpoint) 
    {
    }

    auto get_endpoint() const -> endpoint_type { return this->_D_endpoint; }
};


// ----------------------------------------------------------------------------

#endif
