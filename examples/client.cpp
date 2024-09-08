// examples/client.cpp                                                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/net29/net.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include "demo_task.hpp"
#include "demo_scope.hpp"

namespace ex  = ::beman::execution26;
namespace net = ::beman::net29;

// ----------------------------------------------------------------------------

auto main() -> int
{
    using namespace std::chrono_literals;
    using on_exit = std::unique_ptr<char const, decltype([](auto m){ std::cout << m << "\n";})>;
    net::io_context context;
    demo::scope     scope;

    scope.spawn(std::invoke([](auto& context)->demo::task<> {
        on_exit msg("connecting client done");
        net::ip::tcp::endpoint ep(net::ip::address_v4::loopback(), 12345);
        net::ip::tcp::socket   client(context, ep);

        for (int i{}; i < 5; ++i)
        {
            std::cout << "i=" << i << "\n";
            co_await net::resume_after(context.get_scheduler(), 1s);
        }

        co_await net::async_connect(client);
        std::cout << "connected\n";
        char message[] = "hello, world\n";
        auto b = net::buffer(message);
        if (0 < co_await net::async_send(client, b)) {
            char buffer[20];
            while (auto size = co_await net::async_receive(client, net::buffer(buffer)))
            {
                std::string_view response(buffer, size);
                std::cout << "received='" << response << "'\n";
                if (response.find('\n') != response.npos)
                    break;
            }
        }
    }, context));

    context.run();
}
