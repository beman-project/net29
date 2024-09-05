// include/beman/net29/detail/io_context.hpp                          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_IO_CONTEXT
#define INCLUDED_BEMAN_NET29_DETAIL_IO_CONTEXT

// ----------------------------------------------------------------------------

#include <beman/net29/detail/netfwd.hpp>
#include <beman/net29/detail/context_base.hpp>
#include <beman/net29/detail/io_context_scheduler.hpp>
#include <beman/net29/detail/poll_context.hpp>
#include <beman/net29/detail/container.hpp>
#include <cstdint>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <limits>
#include <cerrno>
#include <csignal>

// ----------------------------------------------------------------------------

namespace beman::net29
{
    class io_context;
}

// ----------------------------------------------------------------------------

class beman::net29::io_context
{
private:
    ::std::unique_ptr<::beman::net29::detail::context_base> _D_owned{new ::beman::net29::detail::poll_context()};
    ::beman::net29::detail::context_base&                   _D_context{*this->_D_owned};

public:
    using scheduler_type = ::beman::net29::detail::io_context_scheduler;
    class executor_type {};

    io_context() { std::signal(SIGPIPE, SIG_IGN); }
    io_context(::beman::net29::detail::context_base& _Context): _D_owned(), _D_context(_Context) {}
    io_context(io_context&&) = delete;

    auto _Make_socket(int _D, int _T, int _P, ::std::error_code& _Error) -> ::beman::net29::detail::socket_id
    {
        return this->_D_context._Make_socket(_D, _T, _P, _Error);
    }
    auto _Release(::beman::net29::detail::socket_id _Id, ::std::error_code& _Error) -> void
    {
        return this->_D_context._Release(_Id, _Error);
    }
    auto _Native_handle(::beman::net29::detail::socket_id _Id) -> ::beman::net29::detail::native_handle_type
    {
        return this->_D_context._Native_handle(_Id);
    }
    auto _Set_option(::beman::net29::detail::socket_id _Id,
                     int _Level,
                     int _Name,
                     void const* _Data,
                     ::socklen_t _Size,
                     ::std::error_code& _Error) -> void
    {
        this->_D_context._Set_option(_Id, _Level, _Name, _Data, _Size, _Error);
    }
    auto _Bind(::beman::net29::detail::socket_id _Id, ::beman::net29::ip::basic_endpoint<::beman::net29::ip::tcp> const& _Endpoint, ::std::error_code& _Error)
    {
        this->_D_context._Bind(_Id, ::beman::net29::detail::endpoint(_Endpoint), _Error);
    }
    auto _Listen(::beman::net29::detail::socket_id _Id, int _No, ::std::error_code& _Error)
    {
        this->_D_context._Listen(_Id, _No, _Error);
    }
    auto get_scheduler() -> scheduler_type { return scheduler_type(&this->_D_context); }

    ::std::size_t run_one() { return this->_D_context.run_one(); }
    ::std::size_t run()
    {
        ::std::size_t _Count{};
        while (::std::size_t _C = this->run_one())
        {
            _Count += _C;
        }
        return _Count;
    }
};

// ----------------------------------------------------------------------------

#endif
