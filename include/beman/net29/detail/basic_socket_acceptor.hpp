// include/beman/net29/detail/basic_socket_acceptor.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_BASIC_SOCKET_ACCEPTOR
#define INCLUDED_BEMAN_NET29_DETAIL_BASIC_SOCKET_ACCEPTOR

#include <beman/net29/detail/io_context.hpp>
#include <beman/net29/detail/socket_category.hpp>
#include <system_error>

// ----------------------------------------------------------------------------

namespace beman::net29
{
    template <typename>
    class basic_socket_acceptor;
}

// ----------------------------------------------------------------------------

template <typename AcceptableProtocol>
class ::beman::net29::basic_socket_acceptor
    : public ::beman::net29::socket_base
{
public:
    using scheduler_type     = ::beman::net29::io_context::scheduler_type;
    using executor_type      = ::beman::net29::io_context::executor_type;
    using native_handle_type = ::beman::net29::detail::native_handle_type;
    using protocol_type      = AcceptableProtocol;
    using endpoint_type      = typename protocol_type::endpoint;
    using socket_type        = typename protocol_type::socket;

private:
    ::beman::net29::io_context&       d_context;
    protocol_type                     d_protocol; 
    ::beman::net29::detail::socket_id d_id{};

private:
    template <typename Fun_t>
    static void dispatch(Fun_t&& fun)
    {
        ::std::error_code error{};
        fun(error);
        if (error)
        {
            throw ::std::system_error(error);
        }
    }

public:
    //explicit basic_socket_acceptor(::beman::net29::io_context&);
    basic_socket_acceptor(::beman::net29::io_context&, protocol_type const& protocol);
    basic_socket_acceptor(::beman::net29::io_context& context, endpoint_type const& endpoint, bool reuse = true)
        : ::beman::net29::socket_base()
        , d_context(context)
        , d_protocol(endpoint.protocol())
        , d_id(::beman::net29::detail::socket_id::invalid)
    {
        this->open(endpoint.protocol());
        if (reuse)
        {
            this->set_option(::beman::net29::socket_base::reuse_address(true));
        }
        this->bind(endpoint);
        this->listen();
    }
    basic_socket_acceptor(::beman::net29::io_context&, protocol_type const&, native_handle_type const&);
    basic_socket_acceptor(basic_socket_acceptor const&) = delete;
    basic_socket_acceptor(basic_socket_acceptor&& other)
        : ::beman::net29::socket_base()
        , d_protocol(other.d_protocol)
        , d_id(::std::exchange(other.d_id, ::beman::net29::detail::socket_id::invalid))
    {
    }
    template<typename OtherProtocol>
    basic_socket_acceptor(::beman::net29::basic_socket_acceptor<OtherProtocol>&&);
    ~basic_socket_acceptor()
    {
        //-dk:TODO assert that there is no outstanding work?
        ::std::error_code error{};
        this->close(error);
    }
    basic_socket_acceptor& operator=(basic_socket_acceptor const&) = delete;
    basic_socket_acceptor& operator=(basic_socket_acceptor&&);
    template<typename OtherProtocol>
    basic_socket_acceptor& operator=(::beman::net29::basic_socket_acceptor<OtherProtocol>&&);

    auto get_context() -> ::beman::net29::io_context& { return this->d_context; }
    auto get_scheduler() noexcept -> scheduler_type
    {
        return this->d_context.get_scheduler();
    }
    executor_type      get_executor() noexcept;
    auto native_handle() -> native_handle_type { return this->d_context.native_handle(this->d_id); }
    auto _native_handle() const -> native_handle_type { return this->d_context.native_handle(this->d_id); }
    auto id() const -> ::beman::net29::detail::socket_id { return this->d_id; }
    auto open(protocol_type const& p = protocol_type()) -> void
    {
        dispatch([this, &p](::std::error_code& error){ this->open(p, error); });
    }
    auto open(protocol_type const& p, ::std::error_code& error) -> void
    {
        if (this->is_open())
        {
            error = ::std::error_code(int(socket_errc::already_open), ::beman::net29::socket_category());
        }
        this->d_id = this->d_context.make_socket(p.family(), p.type(), p.protocol(), error);
    }
    void assign(protocol_type const&, native_handle_type const&);
    void assign(protocol_type const&, native_handle_type const&, ::std::error_code&);
    native_handle_type release();
    native_handle_type release(::std::error_code&);
    auto is_open() const noexcept -> bool { return this->d_id != ::beman::net29::detail::socket_id::invalid; }
    auto close() -> void
    {
        dispatch([this](auto& error){ return this->close(error); });
    }
    auto close(::std::error_code& error) -> void
    {
        //-dk:TODO cancel outstanding work
        if (this->is_open())
        {
            this->d_context.release(this->id(), error);
            this->d_id = ::beman::net29::detail::socket_id::invalid;
        }
    }
    void cancel();
    void cancel(::std::error_code&);
    template<typename SettableSocketOption>
    auto set_option(SettableSocketOption const& option) -> void
    {
        dispatch([this, option](::std::error_code& error){ this->set_option(option, error); });
    }
    template<typename SettableSocketOption>
    auto set_option(SettableSocketOption const& option, ::std::error_code& error) -> void
    {
        this->d_context.set_option(
            this->id(),
            option.level(this->d_protocol),
            option.name(this->d_protocol),
            option.data(this->d_protocol),
            option.size(this->d_protocol),
            error);
    }

    template<typename GettableSocketOption>
    void get_option(GettableSocketOption&) const;
    template<typename gettableSocketOption>
    void get_option(gettableSocketOption&, ::std::error_code&) const;
    template<typename IoControlCommand>
    void io_control(IoControlCommand&);
    template<typename IoControlCommand>
    void io_control(IoControlCommand&, ::std::error_code&);
    void non_blocking(bool);
    void non_blocking(bool, ::std::error_code&);
    bool non_blocking() const;
    void native_non_blocking(bool);
    void native_non_blocking(bool, ::std::error_code&);
    bool native_non_blocking() const;
    auto bind(endpoint_type const& endpoint) -> void
    {
        this->dispatch([this, endpoint](::std::error_code& error){ this->bind(endpoint, error); });
    }
    auto bind(endpoint_type const& endpoint, ::std::error_code& error) -> void
    {
        this->d_context.bind(this->d_id, endpoint, error);
    }
    auto listen(int no = ::beman::net29::socket_base::max_listen_connections) -> void
    {
        dispatch([this, no](auto& error){ this->listen(no, error); });
    }
    auto listen(int no, ::std::error_code& error) -> void
    {
        this->d_context.listen(this->d_id, no, error);
    }
    endpoint_type local_endpoint() const;
    endpoint_type local_endpoint(::std::error_code&) const;
    void enable_connection_aborted(bool);
    bool enable_connection_aborted() const;
    socket_type accept();
    socket_type accept(::std::error_code&);
    socket_type accept(io_context&);
    socket_type accept(io_context&, ::std::error_code&);
    socket_type accept(endpoint_type&);
    socket_type accept(endpoint_type&, ::std::error_code&);
    socket_type accept(::beman::net29::io_context&, endpoint_type&);
    socket_type accept(::beman::net29::io_context&, endpoint_type&, ::std::error_code&);
    void wait(::beman::net29::socket_base::wait_type);
    void wait(::beman::net29::socket_base::wait_type, ::std::error_code&);
};

// ----------------------------------------------------------------------------

#endif
