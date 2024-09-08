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

        if (false) for (int i{}; i < 5; ++i)
        {
            std::cout << "i=" << i << "\n";
            co_await net::resume_after(context.get_scheduler(), 1s);
        }

        if (not co_await (net::async_connect(client)
            | ex::then([](auto&&...){ return true; })
            | ex::upon_error([](auto e){
                if constexpr (std::same_as<std::error_code, decltype(e)>)
                {
                    std::error_code f = e;
                    std::cout << "error_code=" << f.message() << "\n";
                }
                else if constexpr (std::same_as<std::exception_ptr, decltype(e)>)
                    ;
                else
                    static_assert(std::same_as<std::error_code, decltype(e)>);
                return false;
            })))
        {
            co_return;
        }

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
    }, context)
#if 0
    | ex::upon_error([](std::exception_ptr ex) {
        try {
            std::cout << "encountered an error!\n";
            std::rethrow_exception(ex);
        }
        catch (std::system_error const& error) {
            std::cout << "error=" << error.what() << "\n";
        }
        catch (...)
        {
            std::cout << "unknown error\n";
        }
    })
#endif
    );

    context.run();
}
