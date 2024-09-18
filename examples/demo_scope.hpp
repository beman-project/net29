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
        static constexpr bool log_completions{false};
        struct env
        {
            scope* self;

            auto query(ex::get_stop_token_t) const noexcept
            {
                return this->self->source.get_token();
            }
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
                ::std::cerr << "ERROR: demo::scope::job in scope completed with error!\n";
                this->complete();
            }
            auto set_value() && noexcept -> void
            {
                if (log_completions) std::cout << "demo::scope::set_value()\n";
                this->complete();
            }
            auto set_stopped() && noexcept -> void
            {
                if (log_completions) std::cout << "demo::scope::set_stopped()\n";
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
            auto get_env() const noexcept -> env { return {this->self}; }
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
        struct state_base
        {
            state_base* next{};
            virtual auto complete() -> void = 0;
        };
        template <typename Receiver>
        struct state
            : state_base
        {
            using operation_state_concept = ex::operation_state_t;
            Receiver receiver;
            scope*   parent{};
            template <typename R>
            state(R&& receiver, scope* parent)
                : receiver(::std::forward<R>(receiver))
                , parent(parent)
            {
            }
            auto start() noexcept -> void
            {
                if (this->parent->count)
                    this->next = ::std::exchange(this->parent->awaiting, this);
                else
                    this->complete();
            }
            auto complete() -> void override
            {
                ex::set_value(::std::move(receiver));
            }
        };
        struct sender
        {
            using sender_concept = ex::sender_t;
            using completion_signatures
                = ex::completion_signatures<ex::set_value_t()>;
            scope* parent{};
            template <typename Receiver>
            auto connect(Receiver&& receiver)
                -> state<::std::remove_cvref_t<Receiver>>
            {
                return { ::std::forward<Receiver>(receiver), this->parent };
            }
        };

        ex::inplace_stop_source  source;
        std::atomic<std::size_t> count{};
        state_base*              awaiting{};

        auto complete() -> void
        {
            for (auto* n{std::exchange(awaiting, {})}; n; n = n->next)
            {
                n->complete();
            }
        }

    public:
        ~scope()
        {
            if (0u < this->count)
                std::cerr << "ERROR: scope destroyed with live jobs: " << this->count << "\n";
        }
        template <ex::sender Sender>
        auto spawn(Sender&& sender)
        {
            ++this->count;
            new job<Sender>(this, std::forward<Sender>(sender));
        }
        auto stop()
        {
            this->source.request_stop();
        }
        auto on_empty() -> sender
        {
            return {this};
        }
    };
}

// ----------------------------------------------------------------------------

#endif
