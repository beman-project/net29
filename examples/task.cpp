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

namespace
{
    struct result { int value{}; };
    struct error { int value{}; };
}

int main(int ac, char*[])
{
    try
    {
        auto res{ex::sync_wait(::std::invoke([](int ac)-> demo::task<result>
        {
            int i = co_await ex::just(17);
            std::cout << "i=" << i << "\n";
            auto[a, b] = co_await ex::just("hello", "world");
            std::cout << "a=" << a << ", b=" << b << "\n";
            try
            {
                co_await ex::just_error(error{17});
            }
            catch (error const& e)
            {
                std::cout << "error=" << e.value << "\n";
            }
            if (ac == 2)
                co_await ex::just_stopped();
            if (ac == 3)
                throw error{42};
            co_return result{17};
        }, ac))};

        if (res)
        {
            auto[r] = *res;
            std::cout << "after coroutine: r=" << r.value << "\n";
        }
        else
        {
            std::cout << "after coroutine: cancelled\n";
        }
    }
    catch(error const& e)
    {
        ::std::cout << "after coroutine: error=" << e.value << "\n";
    }
    catch(std::exception const& e)
    {
        ::std::cout << "after coroutine: exception=" << e.what() << "\n";
    }
    catch(...)
    {
        ::std::cout << "after coroutine: unknown exception\n";
    }
}
