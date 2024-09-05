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
    ::std::unique_ptr<::beman::net29::detail::context_base> d_owned{new ::beman::net29::detail::poll_context()};
    ::beman::net29::detail::context_base&                   d_context{*this->d_owned};

public:
    using scheduler_type = ::beman::net29::detail::io_context_scheduler;
    class executor_type {};

    io_context() { std::signal(SIGPIPE, SIG_IGN); }
    io_context(::beman::net29::detail::context_base& context): d_owned(), d_context(context) {}
    io_context(io_context&&) = delete;

    auto make_socket(int d, int t, int p, ::std::error_code& error) -> ::beman::net29::detail::socket_id
    {
        return this->d_context.make_socket(d, t, p, error);
    }
    auto release(::beman::net29::detail::socket_id id, ::std::error_code& error) -> void
    {
        return this->d_context.release(id, error);
    }
    auto native_handle(::beman::net29::detail::socket_id id) -> ::beman::net29::detail::native_handle_type
    {
        return this->d_context.native_handle(id);
    }
    auto set_option(::beman::net29::detail::socket_id id,
                     int level,
                     int name,
                     void const* data,
                     ::socklen_t size,
                     ::std::error_code& error) -> void
    {
        this->d_context.set_option(id, level, name, data, size, error);
    }
    auto bind(::beman::net29::detail::socket_id id, ::beman::net29::ip::basic_endpoint<::beman::net29::ip::tcp> const& endpoint, ::std::error_code& error)
    {
        this->d_context.bind(id, ::beman::net29::detail::endpoint(endpoint), error);
    }
    auto listen(::beman::net29::detail::socket_id id, int no, ::std::error_code& error)
    {
        this->d_context.listen(id, no, error);
    }
    auto get_scheduler() -> scheduler_type { return scheduler_type(&this->d_context); }

    ::std::size_t run_one() { return this->d_context.run_one(); }
    ::std::size_t run()
    {
        ::std::size_t count{};
        while (::std::size_t c = this->run_one())
        {
            count += c;
        }
        return count;
    }
};

// ----------------------------------------------------------------------------

#endif
