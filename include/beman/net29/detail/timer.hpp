// include/beman/net29/detail/timer.hpp                               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_TIMER
#define INCLUDED_BEMAN_NET29_DETAIL_TIMER

// ----------------------------------------------------------------------------

#include <beman/net29/detail/netfwd.hpp>
#include <beman/net29/detail/sender.hpp>

// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    struct resume_after_desc;
    struct resume_at_desc;
}

namespace beman::net29::detail
{
    using async_resume_after_t = ::beman::net29::detail::sender_cpo<::beman::net29::detail::resume_after_desc>;
    using async_resume_at_t    = ::beman::net29::detail::sender_cpo<::beman::net29::detail::resume_at_desc>;

    inline constexpr async_resume_after_t async_resume_after{};
    inline constexpr async_resume_at_t    async_resume_at{};
}

// ----------------------------------------------------------------------------

struct beman::net29::detail::resume_after_desc
{
    using operation = ::beman::net29::detail::context_base::resume_after_operation;
    template <typename Scheduler, typename>
    struct data
    {
        using completion_signature = ::beman::net29::detail::ex::set_value_t();

        ::std::remove_cvref_t<Scheduler> d_scheduler;
        ::std::chrono::microseconds       d_duration;

        auto id() const -> ::beman::net29::detail::socket_id { return {}; }
        auto events() const { return decltype(POLLIN)(); }
        auto get_scheduler() { return this->d_scheduler; }
        auto set_value(operation&, auto&& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver));
        }
        auto submit(auto* base) -> bool
        {
            ::std::get<0>(*base) = this->d_duration;
            return this->d_scheduler.resume_after(base);
        }
    };
};

// ----------------------------------------------------------------------------

struct beman::net29::detail::resume_at_desc
{
    using operation = ::beman::net29::detail::context_base::resume_at_operation;
    template <typename Scheduler, typename>
    struct data
    {
        using completion_signature = ::beman::net29::detail::ex::set_value_t();

        ::std::remove_cvref_t<Scheduler>       d_scheduler;
        ::std::chrono::system_clock::time_point d_time;

        auto id() const -> ::beman::net29::detail::socket_id { return {}; }
        auto events() const { return decltype(POLLIN)(); }
        auto get_scheduler() { return this->d_scheduler; }
        auto set_value(operation&, auto&& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver));
        }
        auto submit(auto* base) -> bool
        {
            ::std::get<0>(*base) = this->d_time;
            return this->d_scheduler.resume_at(base);
        }
    };
};

// ----------------------------------------------------------------------------

#endif
