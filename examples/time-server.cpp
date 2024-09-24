// examples/time-server.cpp                                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/net29/net.hpp>
#include "demo_scope.hpp"
#include "demo_task.hpp"
#include <chrono>
#include <functional>
#include <sstream>

namespace net = beman::net29;
using namespace std::chrono_literals;

// ----------------------------------------------------------------------------

namespace
{
    auto use(auto&&...) -> void {}

    auto make_client(auto client, auto scheduler) -> demo::task<>
    {
        auto now{[out = std::ostringstream()]() mutable {
            out.str({});
            out << std::chrono::system_clock::now() << '\n';
            return out.str();
        }};
        auto data = now();

        while (auto n = co_await net::async_send(client, net::buffer(data)))
        {
            std::cout << "n=" << n << " size=" << data.size() << " data=" << data << '\n';
            co_await net::resume_after(scheduler, 1s);
            data = now();
        }
    }
}

int main()
{
    demo::scope     scope;
    net::io_context context;

    scope.spawn(std::invoke([](auto& scope, auto& context) -> demo::task<> {
        net::ip::tcp::endpoint ep(net::ip::address_v4::any(), 12345);
        net::ip::tcp::acceptor server(context, ep);

        while (true) {
            auto[client, addr] = co_await net::async_accept(server);
            use(client, addr);
            scope.spawn(make_client(std::move(client), context.get_scheduler()));
        }

    }, scope, context));

    context.run();
}

