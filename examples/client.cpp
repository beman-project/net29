// examples/client.cpp                                                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/net29/net.hpp>
#include <functional>
#include <iostream>
#include "demo_task.hpp"
#include "demo_scope.hpp"

namespace ex  = ::beman::execution26;
namespace net = ::beman::net29;

// ----------------------------------------------------------------------------

auto main() -> int
{
    net::io_context context;
    demo::scope     scope;

    scope.spawn(std::invoke([](auto& context)->demo::task<> {
        net::ip::tcp::endpoint ep(net::ip::address_v4::loopback(), 12345);
        net::ip::tcp::socket   client(context, ep);
        co_await net::async_connect(client);
        std::cout << "connected\n";
        char message[] = "hello, world\n";
        auto b = net::buffer(message);
        if (0 < co_await net::async_send(client, b)) {
            char buffer[20];
            while (auto size = co_await net::async_receive(client, net::buffer(buffer)))
            {
                std::cout << "received='" << std::string_view(buffer, size) << "'\n";
            }
        }
    }, context));

    context.run();
}
