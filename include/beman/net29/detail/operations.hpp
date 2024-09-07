// include/beman/net29/detail/operations.hpp                          -*-C++-*-
// ----------------------------------------------------------------------------
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// ----------------------------------------------------------------------------

#ifndef INCLUDED_INCLUDE_BEMAN_NET29_DETAIL_OPERATIONS
#define INCLUDED_INCLUDE_BEMAN_NET29_DETAIL_OPERATIONS

#include <beman/net29/detail/context_base.hpp>
#include <beman/net29/detail/sender.hpp>

// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    struct accept_desc;
    struct connect_desc;
    struct send_desc;
    struct send_to_desc;
    struct receive_desc;
    struct receive_from_desc;
}

namespace beman::net29
{
    using async_accept_t
        = ::beman::net29::detail::sender_cpo<::beman::net29::detail::accept_desc>;
    inline constexpr async_accept_t async_accept{};

    using async_connect_t
        = ::beman::net29::detail::sender_cpo<::beman::net29::detail::connect_desc>;
    inline constexpr async_connect_t async_connect{};

    using async_send_t
        = ::beman::net29::detail::sender_cpo<::beman::net29::detail::send_desc>;
    inline constexpr async_send_t async_send{};

    using async_send_to_t
        = ::beman::net29::detail::sender_cpo<::beman::net29::detail::send_to_desc>;
    inline constexpr async_send_to_t async_send_to{};

    using async_receive_t
        = ::beman::net29::detail::sender_cpo<::beman::net29::detail::receive_desc>;
    inline constexpr async_receive_t async_receive{};
    using async_receive_from_t
        = ::beman::net29::detail::sender_cpo<::beman::net29::detail::receive_from_desc>;
    inline constexpr async_receive_from_t async_receive_from{};
}

// ----------------------------------------------------------------------------

struct beman::net29::detail::accept_desc
{
    using operation = ::beman::net29::detail::context_base::accept_operation;
    template <typename Acceptor>
    struct data
    {
        using acceptor_t = ::std::remove_cvref_t<Acceptor>;
        using socket_t = acceptor_t::socket_type;
        using completion_signature
            = ::beman::net29::detail::ex::set_value_t(
                socket_t,
                typename socket_t::endpoint_type
            );

        acceptor_t& d_acceptor;
        data(acceptor_t& a): d_acceptor(a) {}

        auto id() const { return this->d_acceptor.id(); }
        auto events() const { return POLLIN; }
        auto get_scheduler() { return this->d_acceptor.get_scheduler(); }
        auto set_value(operation& o, auto&& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver),
                                 socket_t(this->d_acceptor.get_scheduler().get_context(),
                                           ::std::move(*::std::get<2>(o))),
                                 typename socket_t::endpoint_type(::std::get<0>(o)));
        }
        auto submit(auto* base) -> ::beman::net29::detail::submit_result
        {
            ::std::get<1>(*base) = sizeof(::std::get<0>(*base));
            return this->get_scheduler().accept(base);
        }
    };
};

struct beman::net29::detail::connect_desc
{
    using operation = ::beman::net29::detail::context_base::connect_operation;
    template <typename Socket>
    struct data
    {
        using completion_signature = ::beman::net29::detail::ex::set_value_t();

        Socket& d_socket;

        auto id() const { return this->d_socket.id(); }
        auto events() const { return POLLIN; }
        auto get_scheduler() { return this->d_socket.get_scheduler(); }
        auto set_value(operation&, auto&& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver));
        }
        auto submit(auto* base) -> ::beman::net29::detail::submit_result
        {
            ::std::get<0>(*base) = this->d_socket.get_endpoint();
            return this->d_socket.get_scheduler().connect(base);
        }
    };
};

struct beman::net29::detail::send_desc
{
    using operation = ::beman::net29::detail::context_base::send_operation;
    template <typename Stream_t, typename Buffers>
    struct data
    {
        using completion_signature = ::beman::net29::detail::ex::set_value_t(::std::size_t);

        Stream_t& d_stream;
        Buffers   d_buffers;

        auto id() const { return this->d_stream.id(); }
        auto events() const { return POLLOUT; }
        auto get_scheduler() { return this->d_stream.get_scheduler(); }
        auto set_value(operation& o, auto&& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver),
                                 ::std::move(::std::get<2>(o)));
        }
        auto submit(auto* base) -> ::beman::net29::detail::submit_result
        {
            ::std::get<0>(*base).msg_iov    = this->d_buffers.data();
            ::std::get<0>(*base).msg_iovlen = this->d_buffers.size();
            return this->d_stream.get_scheduler().send(base);
        }
    };
};

struct beman::net29::detail::send_to_desc
{
    using operation = ::beman::net29::detail::context_base::send_operation;
    template <typename Stream_t, typename Buffers, typename Endpoint>
    struct Data
    {
        using completion_signature = ::beman::net29::detail::ex::set_value_t(::std::size_t);

        Stream_t& d_stream;
        Buffers   d_buffers;
        Endpoint  d_endpoint;

        auto id() const { return this->d_stream.id(); }
        auto events() const { return POLLOUT; }
        auto get_scheduler() { return this->d_stream.get_scheduler(); }
        auto set_value(operation& o, auto&& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver), ::std::get<2>(o));
        }
        auto submit(auto* base) -> ::beman::net29::detail::submit_result
        {
            ::std::get<0>(*base).msg_iov     = this->d_buffers.data();
            ::std::get<0>(*base).msg_iovlen  = this->d_buffers.size();
            ::std::get<0>(*base).msg_name    = this->d_endpoint.data();
            ::std::get<0>(*base).msg_namelen = this->d_endpoint.size();
            return this->d_stream.get_scheduler().send(base);
        }
    };
};

struct beman::net29::detail::receive_desc
{
    using operation = ::beman::net29::detail::context_base::receive_operation;
    template <typename Stream_t, typename Buffers>
    struct data
    {
        using completion_signature = ::beman::net29::detail::ex::set_value_t(::std::size_t);

        Stream_t& d_stream;
        Buffers   d_buffers;

        auto id() const { return this->d_stream.id(); }
        auto events() const { return POLLIN; }
        auto get_scheduler() { return this->d_stream.get_scheduler(); }
        auto set_value(operation& o, auto&& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver), ::std::get<2>(o));
        }
        auto submit(auto* base) -> ::beman::net29::detail::submit_result
        {
            ::std::get<0>(*base).msg_iov    = this->d_buffers.data();
            ::std::get<0>(*base).msg_iovlen = this->d_buffers.size();
            return this->d_stream.get_scheduler().receive(base);
        }
    };
};

struct beman::net29::detail::receive_from_desc
{
    using operation = ::beman::net29::detail::context_base::receive_operation;
    template <typename Stream_t, typename Buffers, typename Endpoint>
    struct data
    {
        using completion_signature = ::beman::net29::detail::ex::set_value_t(::std::size_t);

        Stream_t& d_stream;
        Buffers   d_buffers;
        Endpoint  d_endpoint;

        auto id() const { return this->d_stream.id(); }
        auto events() const { return POLLIN; }
        auto get_scheduler() { return this->d_stream.get_scheduler(); }
        auto set_value(operation& o, auto&& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver), ::std::get<2>(o));
        }
        auto submit(auto* base) -> ::beman::net29::detail::submit_result
        {
            ::std::get<0>(*base).msg_iov     = this->d_buffers.data();
            ::std::get<0>(*base).msg_iovlen  = this->d_buffers.size();
            ::std::get<0>(*base).msg_name    = this->d_buffers.data();
            ::std::get<0>(*base).msg_namelen = this->d_buffers.size();
            return this->d_stream.get_scheduler().receive(base);
        }
    };
};


// ----------------------------------------------------------------------------

#endif
