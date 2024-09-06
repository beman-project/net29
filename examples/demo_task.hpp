// examples/demo_task.hpp                                             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_EXAMPLES_DEMO_TASK
#define INCLUDED_EXAMPLES_DEMO_TASK

#include <beman/net29/net.hpp>
#include <exception>
#include <coroutine>
#include <tuple>
#include <optional>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace demo
{
    namespace ex = ::beman::net29::detail::ex;

    template <typename Result = void>
    struct task
    {
        template <typename Env, ex::sender Sender>
        struct sender_awaiter
        {
            struct receiver
            {
                using receiver_concept = ex::receiver_t;

                Env const*     env{};
                sender_awaiter* self{};

                template <typename... Args>
                auto set_value(Args&&... args) noexcept -> void
                {
                    this->self->result.emplace(::std::forward<Args>(args)...);
                    this->self->handle.resume();
                }
                template <typename Error>
                auto set_error(Error&& error) noexcept -> void
                {
                    if constexpr (::std::same_as<::std::decay_t<Error>, ::std::exception_ptr>)
                        this->self->error = error;
                    else
                        this->self->error = ::std::make_exception_ptr(::std::forward<Error>(error));
                    this->self->handle.resume();
                }
                auto set_stopped() noexcept -> void
                {
                    ::std::cout << "set_stopped()\n";
                }
            };
            template <typename... T>
            struct single_or_tuple { using type = ::std::tuple<::std::decay_t<T>...>; };
            template <typename T>
            struct single_or_tuple<T> { using type = ::std::decay_t<T>; };
            template <typename... T>
            using single_or_tuple_t = typename single_or_tuple<T...>::type;
            using value_type
                = ex::value_types_of_t<
                    Sender, Env, single_or_tuple_t, ::std::type_identity_t>;
            using state_type = decltype(ex::connect(::std::declval<Sender>(), std::declval<receiver>()));

            ::std::coroutine_handle<>   handle;
            ::std::exception_ptr        error;
            ::std::optional<value_type> result;
            state_type                  state;

            sender_awaiter(Env* env, Sender sender)
                : state(ex::connect(::std::move(sender), receiver{env, this}))
            {
            }

            auto await_ready() const noexcept -> bool { return false; }
            auto await_suspend(::std::coroutine_handle<> handle) -> void
            {
                this->handle = handle;
                ex::start(this->state);
            }
            auto await_resume()
            {
                if (this->error)
                    std::rethrow_exception(this->error);
                return *this->result;
            }
        };
        template <typename E, typename S>
        sender_awaiter(E const*, S&&) -> sender_awaiter<E, ::std::remove_cvref_t<S>>;

        struct promise_type
        {
            auto initial_suspend() -> ::std::suspend_always { return {}; }
            auto final_suspend() noexcept -> ::std::suspend_always { return {}; }
            auto get_return_object()
            {
                return task{::std::coroutine_handle<promise_type>::from_promise(*this)};
            }
            auto unhandled_exception() -> void { std::terminate(); }
            auto await_transform(ex::sender auto && sender)
            {
                return sender_awaiter(this, sender);
            }
        };

        ::std::coroutine_handle<task<Result>::promise_type> handle;
        auto run()
        {
            ::std::cout << "running task\n";
            this->handle.resume();
        }
    };
}

// ----------------------------------------------------------------------------

#endif
