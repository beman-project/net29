// examples/demo_scope.hpp                                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_EXAMPLES_DEMO_SCOPE
#define INCLUDED_EXAMPLES_DEMO_SCOPE

#include <beman/net29/net.hpp>
#include <atomic>
#include <iostream>
#include <utility>

// ----------------------------------------------------------------------------

namespace demo
{
    namespace ex = ::beman::net29::detail::ex;

    class scope
    {
    private:
        struct env
        {
            scope* self;

            auto query(ex::get_stop_token_t) const { return this->self->source.get_token(); }
        };

        struct job_base
        {
            virtual ~job_base() = default;
        };

        struct receiver
        {
            using receiver_concept = ex::receiver_t;
            scope*    self;
            job_base* state{};

            auto set_error(auto&&) noexcept -> void
            {
                ::std::cerr << "ERROR: job in scope completed with error!\n";
                this->complete();
            }
            auto set_value() && noexcept -> void
            {
                ::std::cerr << "set_value";
                this->complete();
            }
            auto set_stopped() && noexcept -> void
            {
                ::std::cerr << "set_value";
                this->complete();
            }
            auto complete() -> void
            {
                scope* self{this->self};
                delete this->state;
                if (0u == --self->count)
                {
                    self->complete();
                }
            }
            auto get_env() noexcept -> env { return {this->self}; }
        };

        template <typename Sender>
        struct job
            : job_base
        {
            using state_t = decltype(ex::connect(std::declval<Sender&&>(), std::declval<receiver>()));
            state_t state;
            template <typename S>
            job(scope* self, S&& sender)
                : state(ex::connect(::std::forward<S>(sender), receiver{self, this}))
            {
                ex::start(this->state);
            }
        };

        std::atomic<std::size_t> count{};
        ex::inplace_stop_source  source;

        auto complete() -> void {}

    public:
        template <ex::sender Sender>
        auto spawn(Sender&& sender)
        {
            ++this->count;
            new job<Sender>(this, std::forward<Sender>(sender));
        }
    };
}

// ----------------------------------------------------------------------------

#endif
