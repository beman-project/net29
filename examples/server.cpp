// examples/server.cpp     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <iostream>
#include <functional>
#include <string_view>
#include <expected>
#include <beman/execution26/execution.hpp>
#include <beman/net29/net.hpp>
#include "demo_scope.hpp"
#include "demo_task.hpp"

namespace ex  = ::beman::execution26;
namespace net = ::beman::net29;

auto use(auto&&) -> void {}

///timeout(time, sender, ....)
///{
///    return when_any(sender..., resume_after(time) | into_erro())
///}

auto make_client( auto client) -> demo::task<void>
{
    try
    {
        char buffer[8];
        while (auto size = co_await net::async_receive(client, net::buffer(buffer)))
        //while (auto size = co_await timeout(
        //    net::async_receive(client, net::buffer(buffer),
        //    resume_after(5s);
        //) | upon_error[](auto){ return 0; })))
        {
            std::string_view message(+buffer, size);
            std::cout << "received<" << size << ">(" << message << ")\n";
            auto ssize = co_await net::async_send(client, net::const_buffer(buffer, size));
            std::cout << "sent<ssize>(" << ::std::string_view(buffer, ssize) << ")\n";
        }
        std::cout << "client done\n";
    }
    catch(const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << '\n';
    }
}

int main()
{
    std::cout << std::unitbuf;
    std::cout << "example server\n";

    try
    {
        demo::scope            scope;
        net::io_context        context;

        scope.spawn(std::invoke([](auto& scope, auto& context)->demo::task<>{
            net::ip::tcp::endpoint endpoint(net::ip::address_v4::any(), 12345);
            net::ip::tcp::acceptor acceptor(context, endpoint);
            auto[stream, ep] = co_await net::async_accept(acceptor);

            std::cout << "ep=" << ep << "\n";
            scope.spawn(make_client(std::move(stream)));
        }, scope, context));

        context.run();

#if 0
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
