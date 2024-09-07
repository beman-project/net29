// examples/server.cpp     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <iostream>
#include <functional>
#include <string_view>
#include <beman/execution26/execution.hpp>
#include <beman/net29/net.hpp>
#include "demo_scope.hpp"
#include "demo_task.hpp"

namespace ex  = ::beman::execution26;
namespace net = ::beman::net29;

auto use(auto&&) -> void {}

#if 0
template <typename E>
struct error_handler_base
{
    void operator()(E)
    {
        std::cout << "error_handler\n";
    }
};
template <typename... E>
struct error_handler
    : error_handler_base<E>...
{
};

auto make_client(exec::async_scope& scope, auto client) -> exec::task<void>
{
    try
    {
        char buffer[8];
        while (auto size = co_await net::async_receive(client, net::buffer(buffer)))
        {
            std::string_view message(+buffer, size);
            std::cout << "received<" << size << ">(" << message << ")\n";
            if (message.starts_with("exit"))
            {
                std::cout << "exiting\n";
                scope.get_stop_source().request_stop();
            }
            auto ssize = co_await net::async_send(client, net::mutable_buffer(buffer, size));
            std::cout << "sent<ssize>(" << ::std::string_view(buffer, ssize) << ")\n";
        }
        std::cout << "client done\n";
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}
#endif

struct receiver
{
    using receiver_concept = ex::receiver_t;
    auto set_value(auto&&, auto&& ep) && noexcept -> void
    {
        std::cout << "ep=" << ep << "\n";
    }
    auto set_error(auto&& error) && noexcept -> void
    {
        std::cout << "error=" << error << "\n";
    }
    auto set_stopped() && noexcept -> void
    {
        std::cout << "cancelled\n";
    }
};

struct result { int value{}; };
struct error { int value{}; };

int main()
{
    std::cout << std::unitbuf;
    std::cout << "example server\n";

    try
    {
        demo::scope            scope;
        net::io_context        context;
        net::ip::tcp::endpoint endpoint(net::ip::address_v4::any(), 12345);
        net::ip::tcp::acceptor acceptor(context, endpoint);

        auto s{
            net::async_accept(acceptor)
            //| ex::then([](auto&&, auto&&){})
            //| ex::upon_error([](auto&&){})
            | ex::upon_stopped([](auto&&){})
        };
        using comp = decltype(ex::get_completion_signatures(s, ex::empty_env()));
        static_assert(std::same_as<
            ex::completion_signatures<ex::set_value_t()>,
            comp
        >);
#if 0
        scope.spawn(
            net::async_accept(acceptor)
            | ex::then([](auto&&, auto&&){})
            | ex::upon_error([](auto&&){})
            | ex::upon_stopped([](){})
            );
#endif
        context.run();

#if 0
        auto[stream, ep] = *ex::sync_wait(net::async_accept(acceptor));
        std::cout << "sync completed\n";
        std::cout << "ep=" << ep << "\n";
        std::cout << "print completed\n";
#endif
#if 0

        std::cout << "spawning accept\n";
        scope.spawn(std::invoke([](auto& scope, auto& acceptor)->exec::task<void>{
            while (true)
            {
                auto[stream, endpoint] = co_await net::async_accept(acceptor);
                scope.spawn(
                    make_client(scope, std::move(stream))
                    | stdexec::upon_stopped([]{ std::cout << "client cancelled\n"; })
                    );
                std::cout << "accepted a client\n";
            }
        }, scope, acceptor)
        | stdexec::upon_stopped([]{ std::cout << "acceptor cancelled\n"; })
        );

        scope.spawn(std::invoke([](auto scheduler)->exec::task<void>{
            using namespace std::chrono_literals;
            for (int i{}; i < 100; ++i)
            {
                co_await net::async_resume_after(scheduler, 1'000'000us);
                std::cout << "relative timer fired\n";
                co_await net::async_resume_at(scheduler, ::std::chrono::system_clock::now() + 1s);
                std::cout << "absolute timer fired\n";
            }
        }, context.get_scheduler()));

        std::cout << "running context\n";
        context.run();
        std::cout << "running done\n";
#endif
    }
    catch (std::exception const& ex)
    {
        std::cout << "EXCEPTION: " << ex.what() << "\n";
        abort();
    }
}
