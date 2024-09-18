// examples/http-server.cpp                                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/net29/net.hpp>
#include <beman/execution26/execution.hpp>
#include "demo_algorithm.hpp"
#include "demo_scope.hpp"
#include "demo_task.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <string_view>
#include <unordered_map>

namespace ex  = beman::execution26;
namespace net = beman::net29;
using namespace std::chrono_literals;

// ----------------------------------------------------------------------------

std::unordered_map<std::string, std::string> files{
    {"/", "examples/data/index.html"},
    {"/favicon.ico", "examples/data/favicon.ico"},
    {"/logo.png", "examples/data/logo.png"},
};

auto process_request(auto& stream, std::string request, auto done) -> demo::task<>
{
    std::istringstream in(request);
    std::string method, url, version;

    std::string        body;
    std::ostringstream out;
    if ((in >> method >> url >> version) && method == "GET")
    {
        auto it(files.find(url));
        if (it != files.end())
        {
            out << std::ifstream(it->second).rdbuf();
            body = out.str();
            out.str({});
        }
        else if (url == "/exit")
        {
            body = "<html>"
                   "<head><title>exiting</title></head>"
                   "<body>exiting</body>"
                   "</html>\n";
            done();
        }
    }
    out << "HTTP/1.1 " << (body.empty()? "404 not found": "200 found\r\n")
        << "Content-Length: " << body.size() << "\r\n\r\n"
        << body;
    auto response(out.str());
    co_await net::async_send(stream, net::buffer(response));
}

auto timeout(auto scheduler, auto duration, auto sender)
{
    return demo::when_any(
        std::move(sender),
        net::resume_after(scheduler, duration)
        | demo::into_error([]{ return std::error_code(); })
        );
}

auto make_client(auto scheduler, auto stream, auto done) -> demo::task<>
{
    char        buffer[1024];
    std::string request;
    try{
        while (auto n = co_await timeout(scheduler, 3s, net::async_receive(stream, net::buffer(buffer))))
        {
            std::string_view sv(buffer, n);
            request += sv; 
            if (request.npos != sv.find("\r\n\r\n")) {
                co_await process_request(stream, std::move(request), done);
                break;
            }
        }
    }
    catch (...)
    {
        std::cout << "ex: timeout\n";
    }
    std::cout << "client done\n";
}

auto main() -> int
{
    net::io_context context;
    demo::scope     scope;
    net::ip::tcp::endpoint ep(net::ip::address_v4::any(), 12345);
    net::ip::tcp::acceptor server(context, ep);
    std::cout << "listening on " << ep << "\n";

    auto done = [&]{ 
        scope.spawn(
            ex::schedule(context.get_scheduler())
            | ex::then([&]{
                scope.stop();
                //std::cout << "stopped\n";
            })
        );
    };
    scope.spawn(std::invoke([](auto scheduler, auto& scope, auto& server, auto done)
        -> demo::task<>
    {
        while (true)
        {
            auto[stream, address] = co_await net::async_accept(server);
            std::cout << "received connection from " << address << "\n";
            scope.spawn(make_client(scheduler, std::move(stream), done));

        }
    }, context.get_scheduler(), scope, server, done));

    context.run();
    ex::sync_wait(scope.on_empty());
}
