// examples/set_next.cpp                                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/execution.hpp>
#include <beman/net29/net.hpp>
#include <chrono>
#include <iostream>
#include <optional>
#include <utility>

namespace ex = beman::execution26;
namespace net = beman::net29;
using namespace std::chrono_literals;

// ----------------------------------------------------------------------------

struct sequence_sender_t: ex::sender_t {};
struct sequence_receiver_t: ex::receiver_t {};

template <typename R>
concept sequence_receiver
    =  ex::receiver<R>
    && std::derived_from<typename R::receiver_concept, sequence_receiver_t>
    ;

struct set_next_t
{
    template <typename R, typename... A>
    auto operator()(R& r, A&&... a) const { return r.set_next(std::forward<A>(a)...); }
};
inline constexpr set_next_t set_next{};

struct iota_t
{
    template <typename B, typename E, typename R>
    struct state
    {
        struct receiver
        {
            using receiver_concept = ex::receiver_t;
            state* s{};
            auto set_value() && noexcept -> void {
                s->produce_next();
            }
            auto set_error(auto&&) && noexcept -> void {
                ex::set_stopped(std::move(s->rcvr));
            }
            auto set_stopped() && noexcept -> void {
                ex::set_stopped(std::move(s->rcvr));
            }
        };

        using operation_state_concept = ex::operation_state_t;
        using inner_state_t = decltype(ex::connect(set_next(std::declval<R&>(), std::declval<B>()), std::declval<receiver>()));
        B begin;
        E end;
        R rcvr;
        std::optional<inner_state_t> st{};

        auto start() & noexcept -> void { this->produce_next(); }
        auto produce_next()
        {
            if (this->begin == this->end)
            {
                ex::set_value(std::move(this->rcvr));
                return;
            }

            auto s = set_next(this->rcvr, begin);
            ++begin;
            this->st.emplace(std::move(s), receiver{this});
            ex::start(*this->st);
        }
    };
    template <typename B, typename E>
    struct sender
    {
        using sender_concept = sequence_sender_t;
        using completion_signatures = ex::completion_signatures<
            ex::set_value_t(),
            ex::set_stopped_t()
        >;
        B begin;
        E end;

        template <sequence_receiver R>
        auto connect(R&& r) -> state<B, E, std::remove_cvref_t<R>> {
            return { this->begin, this->end, std::forward<R>(r) };
        }
    };

    template <typename B, typename E = B>
    auto operator()(B begin, E end) const {
        return sender<std::remove_cvref_t<B>, std::remove_cvref_t<E>>{begin, end};
    }
};
inline constexpr iota_t iota{};

struct sreceiver
{
    using receiver_concept = sequence_receiver_t;

    std::string                     name;
    decltype(1ms)                   duration;
    net::io_context::scheduler_type scheduler;


    auto set_value() && noexcept -> void {
        std::cout << "sreceiver set_value\n";
    }
    auto set_stopped() && noexcept -> void {
        std::cout << "sreceiver set_stopped\n";
    }
    auto set_next(auto v) {
        std::cout << "sreceiver set_next(" << v << ")\n";
        return net::resume_after(this->scheduler, this->duration)
            | ex::then([this, v](){
                std::cout << "sreceiver(" << this->name << ") executing " << v << "\n";
            });
    }
};

int main()
{
    static_assert(ex::sender<decltype(iota(0, 3))>);
    static_assert(ex::receiver<sreceiver>);
    static_assert(sequence_receiver<sreceiver>);

    net::io_context context;

    auto s1{ex::connect(iota(0, 10), sreceiver{"seq1", 300ms, context.get_scheduler()})};
    ex::start(s1);
    auto s2{ex::connect(iota(0, 10), sreceiver{"seq2", 200ms, context.get_scheduler()})};
    ex::start(s2);

    context.run();
}