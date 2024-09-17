// examples/demo_task.hpp                                             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_EXAMPLES_DEMO_TASK
#define INCLUDED_EXAMPLES_DEMO_TASK

#include <string>
#include <system_error>
#include <exception>
#include <coroutine>
#include <memory>
#include <tuple>
#include <optional>
#include <type_traits>
#include <atomic>
#include <utility>
#include <variant>
#include <version>
#include <iostream>
#include <concepts>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <cstddef>
#include <cassert>
#include <limits>
#include <cstdint>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <array>
#include <compare>
#include <cstring>
#include <ostream>
#include <stdexcept>
#include <chrono>
#include <sys/time.h>
#include <cerrno>
#include <csignal>
#include <algorithm>
// include/beman/net29/detail/buffer.hpp                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_BUFFER
#define INCLUDED_BEMAN_NET29_DETAIL_BUFFER


// ----------------------------------------------------------------------------

namespace beman::net29
{
    enum class stream_errc: int;

    auto stream_category() noexcept -> ::std::error_category const&;

    auto make_error_code(::beman::net29::stream_errc) noexcept -> ::std::error_code;
    auto make_error_condition(::beman::net29::stream_errc) noexcept -> ::std::error_condition;

    struct mutable_buffer;
    struct const_buffer;

    template <typename> struct is_mutable_buffer_sequence;
    template <typename> struct is_const_buffer_sequence;
    template <typename> struct is_dynamic_buffer;

    struct buffer_sequence;

    template <::std::size_t _S>
    auto buffer(char (&)[_S]) -> ::beman::net29::mutable_buffer;
    auto buffer(char*, ::std::size_t) -> ::beman::net29::mutable_buffer;
    auto buffer(char const*, ::std::size_t) -> ::beman::net29::const_buffer;
    template <typename _CT>
        requires requires(_CT&& _C){ _C.data(); _C.size(); }
    auto buffer(_CT&& _C)
    {
        return beman::net29::buffer(_C.data(), _C.size());
    }
}

// ----------------------------------------------------------------------------

enum class beman::net29::stream_errc: int
{
    eof,
    not_found
};

// ----------------------------------------------------------------------------

inline auto beman::net29::stream_category() noexcept -> ::std::error_category const&
{
    struct _Category
        : ::std::error_category
    {
        auto name() const noexcept -> char const* override
        {
            return "stream_error";
        }
        auto message(int) const noexcept -> ::std::string override
        {
            return {};
        }
    };
    static _Category _Rc{};
    return _Rc; 
}

// ----------------------------------------------------------------------------

struct beman::net29::mutable_buffer
{
    ::iovec _Vec;
    mutable_buffer(void* _B, ::std::size_t _L): _Vec{ .iov_base = _B, .iov_len = _L } {}

    auto data() -> ::iovec*      { return &this->_Vec; }
    auto size() -> ::std::size_t { return 1u; }
};

struct beman::net29::const_buffer
{
    ::iovec _Vec;
    const_buffer(void const* _B, ::std::size_t _L): _Vec{ .iov_base = const_cast<void*>(_B), .iov_len = _L } {}

    auto data() -> ::iovec*      { return &this->_Vec; }
    auto size() -> ::std::size_t { return 1u; }
};

template <::std::size_t _S>
inline auto beman::net29::buffer(char (&_B)[_S]) -> ::beman::net29::mutable_buffer
{
    return ::beman::net29::mutable_buffer(_B, _S);
}

inline auto beman::net29::buffer(char* _B, ::std::size_t _Size) -> ::beman::net29::mutable_buffer
{
    return ::beman::net29::mutable_buffer(_B, _Size);
}

inline auto beman::net29::buffer(char const* _B, ::std::size_t _Size) -> ::beman::net29::const_buffer
{
    return ::beman::net29::const_buffer(_B, _Size);
}

// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/endpoint.hpp                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_ENDPOINT
#define INCLUDED_BEMAN_NET29_DETAIL_ENDPOINT


// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    class endpoint;
}

// ----------------------------------------------------------------------------

class beman::net29::detail::endpoint
{
private:
    ::sockaddr_storage d_data{};
    ::socklen_t        d_size{sizeof(::sockaddr_storage)};
public:
    endpoint() = default;
    endpoint(void const* data, ::socklen_t size)
        : d_size(size)
    {
        ::std::memcpy(&this->d_data, data, ::std::min(size, ::socklen_t(sizeof(::sockaddr_storage))));
    }
    template <typename ET>
    endpoint(ET& e): endpoint(e.data(), e.size()) {}

    auto storage()       -> ::sockaddr_storage& { return this->d_data; }
    auto storage() const -> ::sockaddr_storage const& { return this->d_data; }
    auto data()       -> ::sockaddr*        { return reinterpret_cast<::sockaddr*>(&this->d_data); }
    auto data() const -> ::sockaddr const*  { return reinterpret_cast<::sockaddr const*>(&this->d_data); }
    auto size() const  -> ::socklen_t  { return this->d_size; }
    auto size()        -> ::socklen_t& { return this->d_size; }
};

// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/netfwd.hpp                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_NETFWD
#define INCLUDED_BEMAN_NET29_DETAIL_NETFWD


// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    enum socket_id: ::std::uint_least32_t
    {
        invalid = ::std::numeric_limits<::std::uint_least32_t>::max()
    };
    struct context_base;
    using native_handle_type = int;
    inline constexpr native_handle_type invalid_handle{-1};
}

namespace beman::net29
{
    class io_context;
    class socket_base;
    template <typename> class basic_socket;
    template <typename> class basic_stream_socket;
    template <typename> class basic_socket_acceptor;
    namespace ip
    {
        template <typename> class basic_endpoint;
        class tcp;
        class address;
        class address_v4;
        class address_v6;
    }
}


// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/socket_category.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_SOCKET_CATEGORY
#define INCLUDED_BEMAN_NET29_DETAIL_SOCKET_CATEGORY


// ----------------------------------------------------------------------------

namespace beman::net29
{
    enum class socket_errc: int;
    auto socket_category() noexcept -> ::std::error_category const&;
}

// ----------------------------------------------------------------------------

enum class beman::net29::socket_errc: int
{
    already_open = 1,
    not_found
};

auto beman::net29::socket_category() noexcept -> ::std::error_category const&
{
    struct category
        : ::std::error_category
    {
        auto name() const noexcept -> char const* override final { return "socket"; }
        auto message(int error) const -> ::std::string override final
        {
            switch (::beman::net29::socket_errc(error))
            {
            default: return "none";
            case ::beman::net29::socket_errc::already_open: return "already open";
            case ::beman::net29::socket_errc::not_found: return "not found";
            }
        }
    };
    static const category rc{};
    return rc;
}


// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/sorted_list.hpp                         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_SORTED_LIST
#define INCLUDED_BEMAN_NET29_DETAIL_SORTED_LIST


// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    template <typename T, typename = ::std::less<>, typename = decltype([](T* n){ return n->value; })>
    struct sorted_list;
}

// ----------------------------------------------------------------------------

template <typename T, typename Compare, typename Value>
struct beman::net29::detail::sorted_list
{
    using next_t = decltype(::std::declval<T>().next);

    static constexpr auto value{[v = Value()](next_t n){
        return v(static_cast<T*>(n));
    }};
    Compare comp{};
    next_t  head{};

    auto empty() const -> bool { return this->head == nullptr; }
    auto front() const -> T* { return static_cast<T*>(this->head); }
    auto value_or(auto&& v) const { return this->empty()? v: this->value(this->head); }
    auto pop_front() -> T*;
    auto pop_front_or(auto&& v) { return this->empty()? v: this->value(this->pop_front()); }
    auto insert(T* node) -> T*;
    auto erase(next_t) -> next_t;
};

template <typename T, typename Compare, typename Value>
auto beman::net29::detail::sorted_list<T, Compare, Value>::pop_front() -> T*
{
    next_t rc{this->head};
    this->head = rc->next;
    return static_cast<T*>(rc);
}

template <typename T, typename Compare, typename Value>
auto beman::net29::detail::sorted_list<T, Compare, Value>::insert(T* node) -> T*
{
    next_t* it{&this->head};
    while (*it != nullptr && this->comp(this->value(*it), this->value(node)))
        it = &(*it)->next;
    node->next = *it;
    *it = node;
    return node;
}

template <typename T, typename Compare, typename Value>
auto beman::net29::detail::sorted_list<T, Compare, Value>::erase(next_t node) -> next_t
{
    next_t* it{&this->head};
    while (*it != node && *it != nullptr)
        it = &(*it)->next;
    if (*it == node)
    {
        *it = node->next;
        return node;
    }
    return nullptr;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/check_type_alias_exist.hpp        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CHECK_TYPE_ALIAS_EXIST
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CHECK_TYPE_ALIAS_EXIST

// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <template<typename> class> struct check_type_alias_exist;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/inplace_stop_source.hpp           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_INPLACE_STOP_SOURCE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_INPLACE_STOP_SOURCE


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    class inplace_stop_token;
    class inplace_stop_source;
    template<typename CallbackFun> class inplace_stop_callback;
    template <typename CallbackFun>
    inplace_stop_callback(::beman::execution26::inplace_stop_token, CallbackFun)
        -> inplace_stop_callback<CallbackFun>;
}

// ----------------------------------------------------------------------------

class beman::execution26::inplace_stop_token
{
public:
    template <typename CallbackFun>
    using callback_type = ::beman::execution26::inplace_stop_callback<CallbackFun>;

    inplace_stop_token() = default;

    constexpr auto stop_requested() const noexcept  -> bool;
    constexpr auto stop_possible() const noexcept  -> bool;
    auto operator== (inplace_stop_token const&) const -> bool = default;

    auto swap(inplace_stop_token&) noexcept -> void;

private:
    friend class ::beman::execution26::inplace_stop_source;
    template <typename CallbackFun>
    friend class ::beman::execution26::inplace_stop_callback;
    inplace_stop_token(::beman::execution26::inplace_stop_source* source): source(source) {}

    ::beman::execution26::inplace_stop_source* source{};
};

// ----------------------------------------------------------------------------

class beman::execution26::inplace_stop_source
{
    struct callback_base
    {
        callback_base* next{};
        virtual auto call() -> void = 0;
    };

public:
    auto stop_requested() const noexcept -> bool;
    static constexpr auto stop_possible() noexcept -> bool;
    auto get_token() const -> ::beman::execution26::inplace_stop_token;

    auto request_stop() -> bool;

private:
    template <typename CallbackFun>
    friend class ::beman::execution26::inplace_stop_callback;
    ::std::atomic<bool>           stopped{};
    ::std::atomic<callback_base*> running{};
    ::std::thread::id             id{};
    ::std::mutex                  lock;
    callback_base*                callbacks{};

    auto add(callback_base* cb) -> void;
    auto deregister(callback_base* cb) -> void;
};

// ----------------------------------------------------------------------------

template <typename CallbackFun>
class beman::execution26::inplace_stop_callback final
    : public ::beman::execution26::inplace_stop_source::callback_base 
{
public:
    using callback_type = CallbackFun;

    template <typename Init>
    inplace_stop_callback(::beman::execution26::inplace_stop_token, Init&&);
    inplace_stop_callback(inplace_stop_callback&&) = delete;
    ~inplace_stop_callback()
    {
        if (this->source)
        {
            this->source->deregister(this);
        }
    }

private:
    auto call() -> void override;

    CallbackFun                          fun;
    ::beman::execution26::inplace_stop_source* source;
};

// ----------------------------------------------------------------------------

inline constexpr auto beman::execution26::inplace_stop_token::stop_requested() const noexcept  -> bool
{
    return this->source && this->source->stop_requested();
}

inline constexpr auto beman::execution26::inplace_stop_token::stop_possible() const noexcept  -> bool
{
    return this->source;
}

inline auto beman::execution26::inplace_stop_token::swap(inplace_stop_token& other) noexcept -> void
{
    ::std::swap(this->source, other.source);
}

inline auto beman::execution26::inplace_stop_source::stop_requested() const noexcept -> bool
{
    return this->stopped;
}

inline constexpr auto beman::execution26::inplace_stop_source::stop_possible() noexcept -> bool
{
    return true;
}

inline auto beman::execution26::inplace_stop_source::get_token() const -> ::beman::execution26::inplace_stop_token
{
    return ::beman::execution26::inplace_stop_token(const_cast<::beman::execution26::inplace_stop_source*>(this));
}

inline auto beman::execution26::inplace_stop_source::request_stop() -> bool
{
    using relock = ::std::unique_ptr<::std::unique_lock<::std::mutex>, decltype([](auto p){ p->lock(); })>;
    if (false == this->stopped.exchange(true))
    {
        ::std::unique_lock guard(this->lock);
        for (auto it = this->callbacks; it != nullptr; it = this->callbacks)
        {
            this->running   = it;
            this->id        = ::std::this_thread::get_id();
            this->callbacks = it->next;
            {
                relock r(&guard);
                guard.unlock();
                it->call();
            }
            this->running = nullptr;
        }
        return true;
    }
    return false;
}

inline auto beman::execution26::inplace_stop_source::add(callback_base* cb) -> void
{
    if (this->stopped)
    {
        cb->call();
    }
    else
    {
        ::std::lock_guard guard(this->lock);
        cb->next = ::std::exchange(this->callbacks, cb);
    }
}

inline auto beman::execution26::inplace_stop_source::deregister(callback_base* cb) -> void
{
    ::std::unique_lock guard(this->lock);
    if (this->running == cb)
    {
        if (this->id == ::std::this_thread::get_id())
        {
            return;
        }
        guard.unlock();
        while (this->running == cb)
        {
        }
        return;
    }

    for (callback_base** it{&this->callbacks}; *it; it = &(*it)->next)
    {
        if (*it == cb)
        {
            *it = cb->next;
            break;
        }
    }
}

template <typename CallbackFun>
    template <typename Init>
inline beman::execution26::inplace_stop_callback<CallbackFun>::inplace_stop_callback(
    ::beman::execution26::inplace_stop_token token,
    Init&& init)
    : fun(::std::forward<Init>(init))
    , source(token.source)
{
    if (this->source)
    {
        this->source->add(this);
    }
}

template <typename CallbackFun>
inline auto beman::execution26::inplace_stop_callback<CallbackFun>::call() -> void 
{
    this->fun();
}


// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/never_stop_token.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_NEVER_STOP_TOKEN
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_NEVER_STOP_TOKEN

// ----------------------------------------------------------------------------

namespace beman::execution26
{
    class never_stop_token;
}

// ----------------------------------------------------------------------------

class beman::execution26::never_stop_token
{
    struct private_callback_type
    {
        explicit private_callback_type(never_stop_token, auto&&) noexcept {}
    };
public:
    template <typename>
    using callback_type = private_callback_type;

    static constexpr auto stop_requested() noexcept  -> bool { return {}; }
    static constexpr auto stop_possible() noexcept  -> bool { return {}; }
    auto operator== (never_stop_token const&) const -> bool = default;
};

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/nostopstate.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_NOSTOPSTATE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_NOSTOPSTATE

// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct nostopstate_t {
        explicit nostopstate_t() = default;
    };

    inline constexpr nostopstate_t nostopstate{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/stop_callback_for_t.hpp           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_STOP_CALLBACK_FOR
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_STOP_CALLBACK_FOR


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    template<class Token, class CallbackFun>
    using stop_callback_for_t = Token::template callback_type<CallbackFun>;
}

namespace beman::execution26::detail
{
    template<typename CallbackFun, typename Token, typename Initializer = CallbackFun>
    concept stoppable_callback_for
        =  ::std::invocable<CallbackFun>
        && ::std::constructible_from<CallbackFun, Initializer>
        && requires { typename ::beman::execution26::stop_callback_for_t<Token, CallbackFun>; }
        && ::std::constructible_from<::beman::execution26::stop_callback_for_t<Token, CallbackFun>,
                                     Token,
                                     Initializer>
        && ::std::constructible_from<::beman::execution26::stop_callback_for_t<Token, CallbackFun>,
                                     Token&,
                                     Initializer>
        && ::std::constructible_from<::beman::execution26::stop_callback_for_t<Token, CallbackFun>,
                                     Token const&,
                                     Initializer>
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/queryable.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_QUERYABLE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_QUERYABLE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename T>
    concept queryable = ::std::destructible<T>;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/empty_env.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_EMPTY_ENV
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_EMPTY_ENV

// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct empty_env {};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/forwarding_query.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_FORWARDING_QUERY
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_FORWARDING_QUERY


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct forwarding_query_t
    {
        template <typename Object>
            requires requires(Object&& object, forwarding_query_t const& query)
            {
                 { ::std::forward<Object>(object).query(query) } noexcept -> ::std::same_as<bool>;
            }
        constexpr auto operator()(Object&& object) const noexcept -> bool
        {
            return ::std::forward<Object>(object).query(*this);
        }
        template <typename Object>
        constexpr auto operator()(Object&&) const noexcept -> bool
        {
            return ::std::derived_from<::std::remove_cvref_t<Object>, ::beman::execution26::forwarding_query_t>;
        }
    };

    inline constexpr forwarding_query_t forwarding_query{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/movable_value.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_MOVABLE_VALUE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_MOVABLE_VALUE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename T>
    concept movable_value
        =  ::std::move_constructible<::std::decay_t<T>>
        && ::std::constructible_from<::std::decay_t<T>, T>
        && (not ::std::is_array_v<::std::remove_reference_t<T>>)
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/matching_sig.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_MATCHING_SIG
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_MATCHING_SIG


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Fun>
    struct matching_sig_transform
    {
        using type = Fun;
    };
    template <typename Return, typename... Args>
    struct matching_sig_transform<Return(Args...)>
    {
        using type = Return(Args&&...);
    };

    template <typename Fun1, typename Fun2>
    inline constexpr bool matching_sig
        = ::std::same_as<
            typename ::beman::execution26::detail::matching_sig_transform<Fun1>::type,
            typename ::beman::execution26::detail::matching_sig_transform<Fun2>::type
            >
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/as_except_ptr.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_AS_EXCEPT_PTR
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_AS_EXCEPT_PTR


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Error>
    decltype(auto) as_except_ptr(Error&& error)
    {
        if constexpr (::std::same_as<::std::exception_ptr, ::std::decay_t<Error>>)
        {
            assert(error != ::std::exception_ptr());
            return ::std::forward<Error>(error);
        }
        else if constexpr (::std::same_as<::std::error_code, ::std::decay_t<Error>>)
        {
            return ::std::make_exception_ptr(::std::system_error(error));
        }
        else
        {
            return ::std::make_exception_ptr(::std::forward<Error>(error));
        }
    }
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/common.hpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_COMMON
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_COMMON

// ----------------------------------------------------------------------------

#if defined(__GNUC__)
#    pragma GCC diagnostic ignored "-Wmissing-braces"
#    pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

#if defined(__clang__)
#    pragma clang diagnostic ignored "-Wmissing-braces"
#endif

// ----------------------------------------------------------------------------

#if defined(__cpp_deleted_function)
#    define BEMAN_EXECUTION26_DELETE(msg) delete(msg)
#else
#    define BEMAN_EXECUTION26_DELETE(msg) delete
#endif

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/simple_allocator.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SIMPLE_ALLOCATOR
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SIMPLE_ALLOCATOR


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Alloc = bool>
    concept simple_allocator
        = requires(Alloc alloc, ::std::size_t n)
        {
            { *alloc.allocate(n) } -> ::std::same_as<typename Alloc::value_type&>;
            alloc.deallocate(alloc.allocate(n), n);
        }
        && ::std::copy_constructible<Alloc>
        && ::std::equality_comparable<Alloc>
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/decayed_same_as.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_DECAYED_SAME_AS
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_DECAYED_SAME_AS


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    //-dk:TODO thoroughly test the concept
    template <typename T0, typename T1>
    concept decayed_same_as
        = ::std::same_as<::std::remove_cvref_t<T0>, ::std::remove_cvref_t<T1>>
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/operation_state.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_OPERATION_STATE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_OPERATION_STATE


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct operation_state_t {};

    template <typename State>
    concept operation_state
        =  ::std::derived_from<typename State::operation_state_concept,
                               ::beman::execution26::operation_state_t>
        && ::std::is_object_v<State>
        && requires(State& state) { { state.start() } noexcept; }
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/child_type.hpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CHILD_TYPE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CHILD_TYPE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Sender, ::std::size_t I = 0u>
    using child_type = decltype(::std::declval<Sender>().template get<I + 2>());
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/decayed_tuple.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_DECAYED_TUPLE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_DECAYED_TUPLE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename... T>
    using decayed_tuple = ::std::tuple<::std::decay_t<T>...>;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/product_type.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_PRODUCT_TYPE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_PRODUCT_TYPE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <::std::size_t, typename T>
    struct product_type_element
    {
        T value;
        auto operator== (product_type_element const&) const -> bool = default;
    };

    template <typename, typename...>
    struct product_type_base;

    template <::std::size_t... I, typename... T>
    struct product_type_base<::std::index_sequence<I...>, T...>
        : ::beman::execution26::detail::product_type_element<I, T>...
    {
        static constexpr ::std::size_t size() { return sizeof...(T); }

        template <::std::size_t J, typename S>
        static auto element_get(
            ::beman::execution26::detail::product_type_element<J, S>& self)
            noexcept
            -> S&
        {
            return self.value;
        }
        template <::std::size_t J, typename S>
        static auto element_get(
            ::beman::execution26::detail::product_type_element<J, S>&& self)
            noexcept
            -> S
        {
            return ::std::move(self.value);
        }
        template <::std::size_t J, typename S>
        static auto element_get(
            ::beman::execution26::detail::product_type_element<J, S> const& self)
            noexcept
            -> S const&
        {
            return self.value;
        }

        template <::std::size_t J>
        auto get() & -> decltype(auto)
        {
            return this->element_get<J>(*this);
        }
        template <::std::size_t J>
        auto get() && -> decltype(auto)
        {
            return this->element_get<J>(::std::move(*this));
        }
        template <::std::size_t J>
        auto get() const& -> decltype(auto)
        {
            return this->element_get<J>(::std::move(*this));
        }

        auto operator== (product_type_base const&) const -> bool = default;
    };

    template <typename... T>
    struct product_type
        : ::beman::execution26::detail::product_type_base<::std::index_sequence_for<T...>, T...>
    {
    };
    template <typename... T>
    product_type(T&&...) -> product_type<::std::decay_t<T>...>;
}

namespace std
{
    template <typename...T>
    struct tuple_size<::beman::execution26::detail::product_type<T...>>
        : ::std::integral_constant<std::size_t, sizeof...(T)>
    {
    };
    template <::std::size_t I, typename...T>
    struct tuple_element<I, ::beman::execution26::detail::product_type<T...>>
    {
        using type = ::std::decay_t<decltype(::std::declval<::beman::execution26::detail::product_type<T...>>().template get<I>())>;
    };
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/decayed_typeof.hpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_DECAYED_TYPEOF
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_DECAYED_TYPEOF


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <auto&& Tag>
    using decayed_typeof = ::std::decay_t<decltype(Tag)>;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/call_result_t.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CALL_RESULT
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CALL_RESULT


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Fun, typename... Args>
    using call_result_t = decltype(::std::declval<Fun>()(std::declval<Args>()...));
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/join_env.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_JOIN_ENV
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_JOIN_ENV


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Env1, typename Env2>
    class join_env
    {
    private:
        Env1 env1;
        Env2 env2;
    
    public:
        template <typename E1, typename E2>
        join_env(E1&& env1, E2&& env2)
            : env1(::std::forward<E1>(env1))
            , env2(::std::forward<E2>(env2))
        {
        }

        template <typename Query, typename... Args>
            requires
                (requires(Env1&, Query const& query, Args&&... args)
                {
                    env1.query(query, ::std::forward<Args>(args)...);
                }
                || requires(Env2& env2, Query const& query, Args&&... args) {
                    env2.query(query, ::std::forward<Args>(args)...);
                }
                )
        auto query(Query const& query, Args&&... args) -> decltype(auto)
        {
            if constexpr (requires{ env1.query(query, ::std::forward<Args>(args)...); })
            {
                return env1.query(query, ::std::forward<Args>(args)...);
            }
            else
            {
                return env2.query(query, ::std::forward<Args>(args)...);
            }
        }
        template <typename Query, typename... Args>
            requires
                (requires(Env1 const&, Query const& query, Args&&... args)
                {
                    env1.query(query, ::std::forward<Args>(args)...);
                }
                || requires(Env2 const& env2, Query const& query, Args&&... args) {
                    env2.query(query, ::std::forward<Args>(args)...);
                }
                )
        auto query(Query const& query, Args&&... args) const -> decltype(auto)
        {
            if constexpr (requires{ env1.query(query, ::std::forward<Args>(args)...); })
            {
                return env1.query(query, ::std::forward<Args>(args)...);
            }
            else
            {
                return env2.query(query, ::std::forward<Args>(args)...);
            }
        }
    };

    template <typename Env1, typename Env2>
    join_env(Env1&&, Env2&&)
        -> join_env<::std::remove_cvref_t<Env1>, ::std::remove_cvref_t<Env2>>;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/sender_decompose.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_DECOMPOSE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_DECOMPOSE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    struct sender_convert_to_any_t
    {
        template <typename T> constexpr operator T() const;
    };

    template <typename Tag, typename Data, typename Children>
    struct sender_meta
    {
        using tag_type = ::std::remove_cvref_t<Tag>;
        using data_type = ::std::remove_cvref_t<Data>;
        using children_type = ::std::remove_cvref_t<Children>;
    };

    template <typename Tag, typename Data, typename Children>
    struct sender_data
    {
        using tag_type      = ::std::remove_cvref_t<Tag>;
        using data_type     = ::std::remove_cvref_t<Data>;
        using children_type = ::std::remove_cvref_t<Children>;

        tag_type  tag;
        Data&     data;
        Children  children;
    };

    template <typename Sender>
    auto get_sender_data(Sender&& sender)
    {
        #if 0
        //-dk:TODO should use a dynamic/language approach:
        auto&& [tag, data, ... children] = sender;
        return sender_meta<decltype(tag), decltype(data), ::std::tuple<decltype(children)...>>;
        #endif
        using sender_type = ::std::remove_cvref_t<Sender>;
        static constexpr ::beman::execution26::detail::sender_convert_to_any_t  at{};

        if constexpr (requires{sender.template get<0>(); sender.size();})
            return [&sender]<::std::size_t... I>(::std::index_sequence<I...>){
                return ::beman::execution26::detail::sender_data<
                    decltype(sender.template get<0>()),
                    decltype(sender.template get<1>()),
                    decltype(::std::tie(sender.template get<2 + I>()...))
                    >{
                        sender.template get<0>(), sender.template get<1>(), ::std::tie(sender.template get<2 + I>()...)
                    };
            }(::std::make_index_sequence<::std::decay_t<decltype(sender)>::size() - 2u>{});
        else if constexpr (requires{ sender_type{ at, at, at, at, at, at }; })
        {
            auto&& [tag, data, c0, c1, c2, c3] = sender;
            return ::beman::execution26::detail::sender_data<decltype(tag), decltype(data), decltype(::std::tie(c0, c1, c2, c3))>{
                tag, data, ::std::tie(c0, c1, c2, c3)
            };
        }
        else if constexpr (requires{ sender_type{ at, at, at, at, at }; })
        {
            auto&& [tag, data, c0, c1, c2] = sender;
            return ::beman::execution26::detail::sender_data<decltype(tag), decltype(data), decltype(::std::tie(c0, c1, c2))>{
                tag, data, ::std::tie(c0, c1, c2)
            };
        }
        else if constexpr (requires{ sender_type{ at, at, at, at }; })
        {
            auto&& [tag, data, c0, c1] = sender;
            return ::beman::execution26::detail::sender_data<decltype(tag), decltype(data), decltype(::std::tie(c0, c1))>{
                tag, data, ::std::tie(c0, c1)
            };
        }
        else if constexpr (requires{ sender_type{ at, at, at }; })
        {
            auto&& [tag, data, c0] = sender;
            return ::beman::execution26::detail::sender_data<decltype(tag), decltype(data), decltype(::std::tie(c0))>{
                tag, data, ::std::tie(c0)
            };
        }
        else if constexpr (requires{ sender_type{ at, at }; })
        {
            auto&& [tag, data] = sender;
            return ::beman::execution26::detail::sender_data<decltype(tag), decltype(data), ::std::tuple<>>{
                tag, data, ::std::tuple<>()
            };
        }
        else
        {
            return ::beman::execution26::detail::sender_meta<void, void, void>{};
        }
    }

    template <typename Sender>
    auto get_sender_meta(Sender&& sender)
    {
        using type = decltype(get_sender_data(sender));
        return sender_meta<
            typename type::tag_type,
            typename type::data_type,
            typename type::children_type
        >{};
    }
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/callable.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CALLABLE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CALLABLE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Fun, typename... Args>
    concept callable
        = requires(Fun&& fun, Args&&... args)
        {
            ::std::forward<Fun>(fun)(::std::forward<Args>(args)...);
        }
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/forward_like.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_FORWARD_LIKE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_FORWARD_LIKE


// ----------------------------------------------------------------------------
// std::forward_like() doesn't work on some compilers, yet. This header
// provides a work-around.

namespace beman::execution26::detail
{
    template <typename> struct forward_like_helper;

    template <typename T>
    struct forward_like_helper
    {
        template <typename U>
        static auto forward(U&& u) -> ::std::remove_reference_t<U>&&
        {
            return ::std::move(u);
        }
    };
    template <typename T>
    struct forward_like_helper<T&&>
    {
        template <typename U>
        static auto forward(U&& u) -> ::std::remove_cvref_t<U>&&
        {
            return ::std::move(u);
        }
    };
    template <typename T>
    struct forward_like_helper<T&>
    {
        template <typename U>
        static auto forward(U&& u) -> ::std::remove_cvref_t<U>&
        {
            return ::std::forward<U&&>(u);
        }
    };
    template <typename T>
    struct forward_like_helper<T const&&>
    {
        template <typename U>
        static auto forward(U&& u) -> ::std::remove_cvref_t<U> const&&
        {
            return ::std::move(u);
        }
    };
    template <typename T>
    struct forward_like_helper<T const&>
    {
        template <typename U>
        static auto forward(U&& u) -> ::std::remove_cvref_t<U> const&
        {
            return ::std::forward<U&&>(u);
        }
    };

    // The overload own_forward_like is used for testing on systems
    // which actually do provide an implementation.
    template <typename T, typename U>
    auto own_forward_like(U&& u) noexcept -> decltype(auto)
    {
        return ::beman::execution26::detail::forward_like_helper<T>::forward(::std::forward<U>(u));
    }

    template <typename T, typename U>
    auto forward_like(U&& u) noexcept -> decltype(auto)
    {
#if 202207 <= __cpp_lib_forward_like
        return ::std::forward_like<T>(::std::forward<U>(u));
#else
        return ::beman::execution26::detail::forward_like_helper<T>::forward(::std::forward<U>(u));
#endif
    }

}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/type_list.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_TYPE_LIST
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_TYPE_LIST

// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename...> struct type_list {};
}


// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/meta_contains.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_META_CONTAINS
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_META_CONTAINS


// ----------------------------------------------------------------------------

namespace beman::execution26::detail::meta
{
    template <typename T, typename... S>
    inline constexpr bool contains{(::std::same_as<T, S> || ...)};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/meta_prepend.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_META_PREPEND
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_META_PREPEND

// ----------------------------------------------------------------------------

namespace beman::execution26::detail::meta::detail
{
    template <typename, typename>
    struct prepend;

    template <template <typename...> class List, typename H, typename... T>
    struct prepend<H, List<T...>>
    {
        using type = List<H, T...>;
    };
}

namespace beman::execution26::detail::meta
{
    template <typename H, typename Tail>
    using prepend = ::beman::execution26::detail::meta::detail::prepend<H, Tail>::type;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/indirect_meta_apply.hpp           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_INDIRECT_META_APPLY
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_INDIRECT_META_APPLY

// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <bool>
    struct indirect_meta_apply
    {
        template <template<typename...> class T, class... A>
        using meta_apply = T<A...>;
    };
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/decays_to.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_DECAYS_TO
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_DECAYS_TO


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename From, typename To>
    concept decays_to = ::std::same_as<::std::decay_t<From>, To>;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/valid_specialization.hpp          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_VALID_SPECIALIZATION
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_VALID_SPECIALIZATION

// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <template <typename...> class T, typename... Args>
    concept valid_specialization
        = requires { typename T<Args...>; }
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/indices_for.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_INDICES_FOR
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_INDICES_FOR


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Sender>
    using indices_for = ::std::remove_reference_t<Sender>::indices_for;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/container.hpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_CONTAINER
#define INCLUDED_BEMAN_NET29_DETAIL_CONTAINER


// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    template <typename> class container;
}

// ----------------------------------------------------------------------------

template <typename Record>
class beman::net29::detail::container
{
private:
    ::std::vector<::std::variant<::std::size_t, Record>> records;
    ::std::size_t                                        free{};

public:
    auto insert(Record r) -> ::beman::net29::detail::socket_id;
    auto erase(::beman::net29::detail::socket_id id) -> void;
    auto operator[](::beman::net29::detail::socket_id id) -> Record&;
};

// ----------------------------------------------------------------------------

template <typename Record>
inline auto beman::net29::detail::container<Record>::insert(Record r) -> ::beman::net29::detail::socket_id
{
    if (this->free == this->records.size())
    {
        this->records.emplace_back(::std::move(r));
        return ::beman::net29::detail::socket_id(this->free++);
    }
    else
    {
        ::std::size_t rc(std::exchange(this->free, ::std::get<0>(this->records[this->free])));
        this->records[rc] = ::std::move(r);
        return ::beman::net29::detail::socket_id(rc);
    }
}

template <typename Record>
inline auto beman::net29::detail::container<Record>::erase(::beman::net29::detail::socket_id id) -> void
{
    this->records[::std::size_t(id)] = std::exchange(this->free, ::std::size_t(id));
}

template <typename Record>
inline auto beman::net29::detail::container<Record>::operator[](::beman::net29::detail::socket_id id) -> Record&
{
    assert(this->records[::std::size_t(id)].index() == 1u);
    return ::std::get<1>(this->records[::std::size_t(id)]);
}

// ----------------------------------------------------------------------------

#endif// include/beman/net29/detail/internet.hpp                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_INTERNET
#define INCLUDED_BEMAN_NET29_DETAIL_INTERNET


// ----------------------------------------------------------------------------

namespace beman::net29::ip
{
    using port_type = ::std::uint_least16_t;

    class tcp;
    class address_v4;
    class address_v6;
    class address;
    template <typename> class basic_endpoint;
}

// ----------------------------------------------------------------------------

class beman::net29::ip::tcp
{
private:
    int d_family;

    constexpr tcp(int f): d_family(f) {}

public:
    using endpoint = basic_endpoint<tcp>;
    using socket   = basic_stream_socket<tcp>;
    using acceptor = basic_socket_acceptor<tcp>;

    tcp() = delete;

    static constexpr auto v4() -> tcp { return tcp(PF_INET); }
    static constexpr auto v6() -> tcp { return tcp(PF_INET6); }

    constexpr auto family() const -> int { return this->d_family; }
    constexpr auto type() const -> int { return SOCK_STREAM; }
    constexpr auto protocol() const -> int { return IPPROTO_TCP; }
};

// ----------------------------------------------------------------------------

class beman::net29::ip::address_v4
{
public:
    using uint_type = uint_least32_t;
    struct bytes_type;

private:
    uint_type d_address;

public:
    constexpr address_v4() noexcept: d_address() {}
    constexpr address_v4(address_v4 const&) noexcept = default;
    constexpr address_v4(bytes_type const&);
    explicit constexpr address_v4(uint_type a)
        : d_address(a)
    {
        if (!(a <= 0xFF'FF'FF'FF))
        {
            throw ::std::out_of_range("IPv4 address is out of range");
        }
    }

    auto operator=(const address_v4& a) noexcept -> address_v4& = default;

    constexpr auto is_unspecified() const noexcept -> bool { return this->to_uint() == 0u; }
    constexpr auto is_loopback() const noexcept -> bool { return (this->to_uint() & 0xFF'00'00'00) == 0x7F'00'00'00; }
    constexpr auto is_multicast() const noexcept -> bool{ return (this->to_uint() & 0xF0'00'00'00) == 0xE0'00'00'00; }
    constexpr auto to_bytes() const noexcept -> bytes_type;
    constexpr auto to_uint() const noexcept -> uint_type  { return this->d_address; }
    template<typename Allocator = ::std::allocator<char>>
    auto to_string(const Allocator& = Allocator()) const
        -> ::std::basic_string<char, ::std::char_traits<char>, Allocator>;

    static constexpr auto any() noexcept -> address_v4 { return address_v4(); }
    static constexpr auto loopback() noexcept -> address_v4 { return address_v4(0x7F'00'00'01u); }
    static constexpr auto broadcast() noexcept -> address_v4 { return address_v4(0xFF'FF'FF'FFu); }

    friend ::std::ostream& operator<< (::std::ostream& out, address_v4 const& a)
    {
        return out << ((a.d_address >> 24) & 0xFFu) << '.'
                    << ((a.d_address >> 16) & 0xFFu) << '.'
                    << ((a.d_address >>  8) & 0xFFu) << '.'
                    << ((a.d_address >>  0) & 0xFFu)
            ;
    }
};

#if 0
constexpr bool operator==(const address_v4& a, const address_v4& b) noexcept;
constexpr bool operator!=(const address_v4& a, const address_v4& b) noexcept;
constexpr bool operator< (const address_v4& a, const address_v4& b) noexcept;
constexpr bool operator> (const address_v4& a, const address_v4& b) noexcept;
constexpr bool operator<=(const address_v4& a, const address_v4& b) noexcept;
constexpr bool operator>=(const address_v4& a, const address_v4& b) noexcept;
// 21.5.6, address_v4 creation:
constexpr address_v4 make_address_v4(const address_v4::bytes_type& bytes);
constexpr address_v4 make_address_v4(address_v4::uint_type val);
constexpr address_v4 make_address_v4(v4_mapped_t, const address_v6& a);
address_v4 make_address_v4(const char* str);
address_v4 make_address_v4(const char* str, error_code& ec) noexcept;
address_v4 make_address_v4(const string& str);
address_v4 make_address_v4(const string& str, error_code& ec) noexcept;
address_v4 make_address_v4(string_view str);
address_v4 make_address_v4(string_view str, error_code& ec) noexcept;
// 21.5.7, address_v4 I/O:
template<class CharT, class Traits>
basic_ostream<CharT, Traits>& operator<<(
basic_ostream<CharT, Traits>& os, const address_v4& addr);
#endif

// ----------------------------------------------------------------------------

class beman::net29::ip::address_v6
{
public:
    struct bytes_type
        : ::std::array<unsigned char, 16>
    {
        template <typename... T>
        explicit constexpr bytes_type(T... t)
            : std::array<unsigned char, 16>{{ static_cast<unsigned char>(t)... }}
        {
        }
    };

private:
    bytes_type d_bytes;

public:
    static constexpr auto any() noexcept -> address_v6;
    static constexpr auto loopback() noexcept -> address_v6;

    constexpr address_v6() noexcept;
    constexpr address_v6(address_v6 const&) noexcept = default;
    constexpr address_v6(unsigned char const (&addr)[16]) noexcept
    {
        ::std::memcpy(d_bytes.data(), addr, 16);
    }

    auto operator= (address_v6 const&) noexcept -> address_v6& = default;
    constexpr auto operator== (address_v6 const&) const -> bool = default;
    constexpr auto operator<=> (address_v6 const&) const -> ::std::strong_ordering;

    auto get_address(::sockaddr_in6& addr, ::beman::net29::ip::port_type port) const
        -> ::socklen_t
    {
        addr.sin6_family = AF_INET6;
        addr.sin6_port = htons(port);
        addr.sin6_flowinfo = 0;
        ::std::memcpy(addr.sin6_addr.s6_addr, this->d_bytes.data(), 16);
        addr.sin6_scope_id = 0;
        return sizeof(::sockaddr_in6);
    }

    constexpr auto is_unspecified() const noexcept -> bool;
    constexpr auto is_loopback() const noexcept -> bool;
    constexpr auto is_multicast() const noexcept -> bool;
    constexpr auto is_link_local() const noexcept -> bool;
    constexpr auto is_site_local() const noexcept -> bool;
    constexpr auto is_v4_mapped() const noexcept -> bool;
    constexpr auto is_multicast_node_local() const noexcept -> bool;
    constexpr auto is_multicast_link_local() const noexcept -> bool;
    constexpr auto is_multicast_site_local() const noexcept -> bool;
    constexpr auto is_multicast_org_local() const noexcept -> bool;
    constexpr auto is_multicast_global() const noexcept -> bool;
    constexpr auto to_bytes() const noexcept -> bytes_type;
    template <typename Allocator = ::std::allocator<char>>
    auto to_string(Allocator const& = {}) const
        -> ::std::basic_string<char, ::std::char_traits<char>, Allocator>;

    friend ::std::ostream& operator<< (::std::ostream& out, address_v6 const&)
    {
        //-dk:TODO
        return out << "<TODO>::1";
    }
};

inline constexpr beman::net29::ip::address_v6::address_v6() noexcept
    : d_bytes()
{
}

inline constexpr auto beman::net29::ip::address_v6::any() noexcept
    -> ::beman::net29::ip::address_v6
{
    return ::beman::net29::ip::address_v6();
}

inline constexpr auto beman::net29::ip::address_v6::loopback() noexcept
    -> ::beman::net29::ip::address_v6
{
    return ::beman::net29::ip::address_v6();
}

// ----------------------------------------------------------------------------

class beman::net29::ip::address
{
private:
    union address_t
    {
        ::sockaddr_storage storage;
        ::sockaddr_in      inet;
        ::sockaddr_in6     inet6;
    };
    
    address_t d_address;

public:
    constexpr address() noexcept
        : d_address()
    {
        this->d_address.storage.ss_family = PF_INET;
    }
    constexpr address(address const&) noexcept = default;
    /*-dk:TODO constexpr*/ address(::beman::net29::ip::address_v4 const& address) noexcept
    {
        this->d_address.inet.sin_family = AF_INET;
        this->d_address.inet.sin_addr.s_addr = htonl(address.to_uint());
        this->d_address.inet.sin_port = 0xFF'FF;
    }
    /*-dk:TODO constexpr*/ address(::beman::net29::ip::address_v6 const& address) noexcept
    {
        address.get_address(this->d_address.inet6, 0xFF'FF);
    }

    auto operator=(address const&) noexcept -> address& = default;
    auto operator=(::beman::net29::ip::address_v4 const&) noexcept -> address&;
    auto operator=(::beman::net29::ip::address_v6 const&) noexcept -> address&;

    auto data() const -> ::sockaddr_storage const& { return this->d_address.storage; }
    constexpr auto is_v4() const noexcept -> bool { return this->d_address.storage.ss_family == PF_INET; }
    constexpr auto is_v6() const noexcept -> bool { return this->d_address.storage.ss_family == PF_INET6; }
    /*constexpr -dk:TODO*/ auto to_v4() const -> ::beman::net29::ip::address_v4
    {
        return ::beman::net29::ip::address_v4(ntohl(reinterpret_cast<::sockaddr_in const&>(this->d_address.storage).sin_addr.s_addr));
    }
    constexpr auto to_v6() const -> ::beman::net29::ip::address_v6
    {
        return ::beman::net29::ip::address_v6(this->d_address.inet6.sin6_addr.s6_addr);
    }
    constexpr auto is_unspecified() const noexcept -> bool;
    constexpr auto is_loopback() const noexcept -> bool;
    constexpr auto is_multicast() const noexcept -> bool;
    template<class Allocator = ::std::allocator<char>>
    auto to_string(Allocator const& = Allocator()) const
        -> ::std::basic_string<char, ::std::char_traits<char>, Allocator>;
    friend ::std::ostream& operator<< (::std::ostream& out, address const& a)
    {
        if (a.is_v4())
            return out << a.to_v4();
        else
            return out << a.to_v6();
    }
};

// ----------------------------------------------------------------------------

template <typename Protocol>
class beman::net29::ip::basic_endpoint
    : public ::beman::net29::detail::endpoint
{
public:
    using protocol_type = Protocol;

    constexpr basic_endpoint() noexcept
        : basic_endpoint(::beman::net29::ip::address(), ::beman::net29::ip::port_type())
    {
    }
    constexpr basic_endpoint(::beman::net29::detail::endpoint const& ep) noexcept
        : ::beman::net29::detail::endpoint(ep)
    {
    }
    constexpr basic_endpoint(const protocol_type&, ::beman::net29::ip::port_type) noexcept;
    constexpr basic_endpoint(const ip::address& address, ::beman::net29::ip::port_type port) noexcept
        : ::beman::net29::detail::endpoint(&address.data(), address.is_v4()? sizeof(::sockaddr_in): sizeof(::sockaddr_in6))
    {
        (address.is_v4()
         ? reinterpret_cast<::sockaddr_in&>(this->storage()).sin_port
         : reinterpret_cast<::sockaddr_in6&>(this->storage()).sin6_port) = htons(port);
    }

    constexpr auto protocol() const noexcept -> protocol_type
    {
        return this->storage().ss_family == PF_INET? ::beman::net29::ip::tcp::v4(): ::beman::net29::ip::tcp::v6();
    }
    /*-dk:TODO constexpr*/ auto address() const noexcept -> ::beman::net29::ip::address
    {
        switch (this->storage().ss_family)
        {
        default: return {};
        case PF_INET: return ::beman::net29::ip::address_v4(ntohl(reinterpret_cast<::sockaddr_in const&>(this->storage()).sin_addr.s_addr));
        //-dk:TODO case PF_INET6: return ::beman::net29::ip::address_v6(reinterpret_cast<::sockaddr_in6 const&>(this->storage()).sin6_addr.s_addr);
        }
    }
    auto address(::beman::net29::ip::address const&) noexcept -> void;
    constexpr auto port() const noexcept -> ::beman::net29::ip::port_type
    {
        switch (this->storage().ss_family)
        {
            default: return {};
            case PF_INET: return ntohs(reinterpret_cast<::sockaddr_in const&>(this->storage()).sin_port);
            case PF_INET6: return ntohs(reinterpret_cast<::sockaddr_in6 const&>(this->storage()).sin6_port);
        }
    }
    auto port(::beman::net29::ip::port_type) noexcept -> void;

    auto size() const -> ::socklen_t
    {
        return this->storage().ss_family == PF_INET? sizeof(::sockaddr_in): sizeof(::sockaddr_in6);
    }

    friend ::std::ostream& operator<< (std::ostream& out, basic_endpoint const& ep)
    {
        return out << ep.address() << ":" << ep.port();
    }
};

// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/io_base.hpp                             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_IO_BASE
#define INCLUDED_BEMAN_NET29_DETAIL_IO_BASE


// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    enum class submit_result { ready, submit, error };
    auto  operator<< (::std::ostream&,
                      ::beman::net29::detail::submit_result) -> ::std::ostream&;

    struct io_base;
    template <typename> struct io_operation;
}

// ----------------------------------------------------------------------------

inline auto beman::net29::detail::operator<< (
    ::std::ostream& out,
    ::beman::net29::detail::submit_result result) -> ::std::ostream&
{
    switch (result)
    {
    case ::beman::net29::detail::submit_result::ready:  return out << "ready";
    case ::beman::net29::detail::submit_result::submit: return out << "submit";
    case ::beman::net29::detail::submit_result::error:  return out << "error";

    }
    return out << "<unknown>";
}

// ----------------------------------------------------------------------------
// The struct io_base is used as base class of operation states. Objects of
// this type are also used to kick off the actual work once a readiness
// indication was received.

struct beman::net29::detail::io_base
{
    using extra_t = ::std::unique_ptr<void, auto(*)(void*)->void>;
    using work_t  = auto(*)(::beman::net29::detail::context_base&, io_base*)
        -> ::beman::net29::detail::submit_result;

    io_base*                              next{nullptr}; // used for an intrusive list
    ::beman::net29::detail::context_base* context{nullptr};
    ::beman::net29::detail::socket_id     id;            // the entity affected
    int                                   event;         // mask for expected events
    work_t                                work;
    extra_t                               extra{nullptr, +[](void*){}};

    io_base(::beman::net29::detail::socket_id id, int event): id(id), event(event) {}

    virtual auto complete() -> void = 0;
    virtual auto error(::std::error_code) -> void = 0;
    virtual auto cancel() -> void = 0;
};


// ----------------------------------------------------------------------------
// The struct io_operation is an io_base storing operation specific data.

template <typename Data>
struct beman::net29::detail::io_operation
    : io_base
    , Data
{
    template <typename D = Data>
    io_operation(::beman::net29::detail::socket_id id, int event, D&& a = Data())
        : io_base(id, event)
        , Data(::std::forward<D>(a))
    {
    }
};


// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/socket_base.hpp                         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_SOCKET_BASE
#define INCLUDED_BEMAN_NET29_DETAIL_SOCKET_BASE

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

class beman::net29::socket_base
{
public:
    template <typename Value, int Level, int Name>
    class socket_option
    {
    private:
        Value d_value;
    
    public:
        explicit socket_option(Value v): d_value(v) {}
        Value value() const { return this->d_value; }
        template <typename Protocol> auto data(Protocol&&) const -> Value const* { return &this->d_value; }
        template <typename Protocol> auto data(Protocol&&)       -> Value const* { return &this->d_value; }
        template <typename Protocol> constexpr auto level(Protocol&&) const -> int { return Level; }
        template <typename Protocol> constexpr auto name(Protocol&&) const -> int { return Name; }
        template <typename Protocol> constexpr auto size(Protocol&&) const -> ::socklen_t { return sizeof(Value); }
    };
    class broadcast;
    class debug;
    class do_not_route;
    class keep_alive;
    class linger;
    class out_of_band_inline;
    class receive_buffer_size;
    class receive_low_watermark;
    class reuse_address
        : public socket_option<int, SOL_SOCKET, SO_REUSEADDR>
    {
    public:
        explicit reuse_address(bool value): socket_option(value) {}
        explicit operator bool() const { return this->value(); }
    };
    class send_buffer_size;
    class send_low_watermark;

    using shutdown_type = int; //-dk:TODO
    static constexpr shutdown_type shutdown_receive{1};
    static constexpr shutdown_type shutdown_send{2};
    static constexpr shutdown_type shutdown_both{3};

    using wait_type = int; //-dk:TODO
    static constexpr wait_type wait_read{1};
    static constexpr wait_type wait_write{2};
    static constexpr wait_type wait_error{3};

    using message_flags = int; //-dk:TODO
    static constexpr message_flags message_peek{1};
    static constexpr message_flags message_out_of_band{2};
    static constexpr message_flags message_do_not_route{3};

    static constexpr int max_listen_connections{SOMAXCONN};

protected:
    socket_base() = default;
    ~socket_base() = default;
};

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/stop_source.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_STOP_SOURCE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_STOP_SOURCE


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    class stop_token;
    class stop_source;
    template<typename CallbackFun> class stop_callback;
    template <typename CallbackFun>
    stop_callback(::beman::execution26::stop_token, CallbackFun) -> stop_callback<CallbackFun>;
}

namespace beman::execution26::detail
{
    struct stop_state;
    struct stop_callback_base;
}
// ----------------------------------------------------------------------------

struct beman::execution26::detail::stop_state
{
    ::std::atomic<bool>                             stop_requested{};
    ::std::atomic<::std::size_t>                    sources{};
    ::std::mutex                                    lock{};
    beman::execution26::detail::stop_callback_base* callbacks{};
    ::std::atomic<bool>                             executing{};

    auto stop_possible() const -> bool { return this->sources != 0 || this->stop_requested; }
};

// ----------------------------------------------------------------------------

struct beman::execution26::detail::stop_callback_base
{
private:
    using stop_state = ::beman::execution26::detail::stop_state;
    ::std::shared_ptr<stop_state> state;

    virtual auto do_call() -> void = 0;

protected:
    stop_callback_base(::beman::execution26::stop_token const&);
    ~stop_callback_base();

public:
    auto call() -> void;
    auto setup() -> void;
    auto deregister() -> void;

    stop_callback_base* next{};
    ::std::thread::id   id{};
};

// ----------------------------------------------------------------------------

class beman::execution26::stop_source
{
private:
    using stop_state = ::beman::execution26::detail::stop_state;

    ::std::shared_ptr<stop_state> state{::std::make_shared<stop_state>()};

public:
    using stop_token = ::beman::execution26::stop_token;

    stop_source();
    explicit stop_source(::beman::execution26::nostopstate_t) noexcept;
    stop_source(stop_source const&);
    auto operator= (stop_source const&) -> stop_source&;
    ~stop_source();

    auto swap(stop_source&) noexcept -> void;
    auto get_token() const -> stop_token;
    auto stop_requested() const noexcept -> bool;
    auto stop_possible() const noexcept -> bool;
    auto request_stop() noexcept -> bool;
};

// ----------------------------------------------------------------------------

class beman::execution26::stop_token
{
private:
    friend ::beman::execution26::stop_source;
    friend ::beman::execution26::detail::stop_callback_base;
    ::std::shared_ptr<::beman::execution26::detail::stop_state> state;

    stop_token(::std::shared_ptr<::beman::execution26::detail::stop_state>);

public:
    template <typename Fun>
    using callback_type = ::beman::execution26::stop_callback<Fun>;

    stop_token() = default;

    auto swap(stop_token& other) noexcept -> void;
    [[nodiscard]] auto stop_requested() const noexcept -> bool;
    [[nodiscard]] auto stop_possible() const noexcept -> bool;

    [[nodiscard]] auto operator== (stop_token const&) const noexcept -> bool = default;
};

// ----------------------------------------------------------------------------

template<typename CallbackFun>
class beman::execution26::stop_callback final
    : private CallbackFun
    , beman::execution26::detail::stop_callback_base
{
private:
    static_assert(::std::invocable<CallbackFun>);
    static_assert(::std::destructible<CallbackFun>);

    using stop_token = ::beman::execution26::stop_token;

    auto do_call() -> void override
    {
        (*this)();
    }

public:
    using callback_type = CallbackFun;

    template <typename Initializer>
    stop_callback(stop_token const& token, Initializer&& init)
        noexcept(::std::is_nothrow_constructible_v<CallbackFun, Initializer>)
        requires(::std::constructible_from<CallbackFun, Initializer>)
        : CallbackFun(::std::forward<Initializer>(init))
        , stop_callback_base(token)
    {
        this->setup();
    }
    template <typename Initializer>
    stop_callback(stop_token&& token, Initializer&& init)
        noexcept(::std::is_nothrow_constructible_v<CallbackFun, Initializer>)
        requires(::std::is_constructible_v<CallbackFun, Initializer>)
        : CallbackFun(::std::forward<Initializer>(init))
        , stop_callback_base(::std::move(token))
    {
        this->setup();
    }
    ~stop_callback()
    {
        this->deregister();
    }
    stop_callback(stop_callback&&) = delete;
};

// ----------------------------------------------------------------------------

inline beman::execution26::detail::stop_callback_base::stop_callback_base(
    ::beman::execution26::stop_token const& token)
    : state(token.state)
{
}

inline beman::execution26::detail::stop_callback_base::~stop_callback_base()
{
}

inline auto beman::execution26::detail::stop_callback_base::setup() -> void
{
    if (this->state)
    {
        {
            ::std::lock_guard guard(this->state->lock);
            if (!this->state->stop_requested)
            {
                this->next = ::std::exchange(this->state->callbacks, this);
                return;
            }
        }
        this->call();
    }
}

inline auto beman::execution26::detail::stop_callback_base::deregister() -> void
{
    if (this->state)
    {
        ::std::unique_lock guard(this->state->lock);
        if (this->state->executing && this->id != ::std::this_thread::get_id())
        {
            using lock_again = decltype([](auto p){ p->lock(); });
            ::std::unique_ptr<decltype(guard), lock_again> relock(&guard);
            relock->unlock();
            while (this->state->executing)
                ;
        }
        for (auto next = &this->state->callbacks; *next; next = &this->next)
        {
            if (*next == this)
            {
                *next = this->next;
                break;
            }
        }
    }
}

inline auto beman::execution26::detail::stop_callback_base::call() -> void
{
    this->do_call();
}

inline beman::execution26::stop_token::stop_token(::std::shared_ptr<::beman::execution26::detail::stop_state> state)
    : state(::std::move(state))
{
}

inline auto beman::execution26::stop_token::swap(stop_token& other) noexcept -> void
{
    this->state.swap(other.state);
}

inline auto beman::execution26::stop_token::stop_requested() const noexcept -> bool
{
    return this->state && this->state->stop_requested;
}

inline auto beman::execution26::stop_token::stop_possible() const noexcept -> bool
{
    return this->state && this->state->stop_possible();
}

// ----------------------------------------------------------------------------

inline beman::execution26::stop_source::stop_source()
{
    ++this->state->sources;
}

inline beman::execution26::stop_source::stop_source(::beman::execution26::nostopstate_t) noexcept
    : state()
{
}

inline beman::execution26::stop_source::stop_source(stop_source const& other)
    : state(other.state)
{
    ++this->state->sources;
}

inline auto beman::execution26::stop_source::operator= (stop_source const& other) -> stop_source&
{
    --this->state->sources;
    this->state = other.state;
    ++this->state->sources;
    return *this;
}

inline beman::execution26::stop_source::~stop_source()
{
    this->state && --this->state->sources;
}

inline auto beman::execution26::stop_source::swap(::beman::execution26::stop_source& other) noexcept -> void
{
    this->state.swap(other.state);
}

inline auto beman::execution26::stop_source::get_token() const -> stop_token
{
    return stop_token{this->state};
}

inline auto beman::execution26::stop_source::stop_requested() const noexcept -> bool
{
    return this->state && this->state->stop_requested;
}

inline auto beman::execution26::stop_source::stop_possible() const noexcept -> bool
{
    return true && this->state;
}

inline auto beman::execution26::stop_source::request_stop() noexcept -> bool
{
    using release = decltype([](auto p){ *p = false; });
    using lock_again = decltype([](auto p){ p->lock(); });

    if (this->state && not this->state->stop_requested.exchange(true))
    {
        ::std::unique_lock guard(this->state->lock);
        while (this->state->callbacks)
        {
            auto front = ::std::exchange(this->state->callbacks, this->state->callbacks->next);
            ::std::unique_ptr<::std::atomic<bool>, release> reset(&state->executing);
            *reset = true;
            front->id = ::std::this_thread::get_id();
            ::std::unique_ptr<decltype(guard), lock_again> relock(&guard);
            relock->unlock();
            front->call();
        }
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/stoppable_token.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_STOPPABLE_TOKEN
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_STOPPABLE_TOKEN


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    template<typename Token>
    concept stoppable_token
        =   requires(Token const& token)
            {
                typename ::beman::execution26::detail::check_type_alias_exist<Token::template callback_type>;
                { token.stop_requested() } noexcept -> ::std::same_as<bool>;
                { token.stop_possible() } noexcept -> ::std::same_as<bool>;
                { Token(token) } noexcept;
            }
        &&  ::std::copyable<Token>
        &&  ::std::equality_comparable<Token>
        &&  ::std::swappable<Token>
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/get_allocator.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_ALLOCATOR
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_ALLOCATOR


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct get_allocator_t
    {
        template <typename Object>
            requires (not requires(Object&& object, get_allocator_t const& tag)
            {
                ::std::as_const(object).query(tag);
            })
        auto operator()(Object&&) const
            = BEMAN_EXECUTION26_DELETE("the object requires a const query(get_allocator_t) overload");
        template <typename Object>
            requires (not requires(Object const& object, get_allocator_t const& tag)
            {
                object.query(tag);
            })
        auto operator()(Object&&) const
            = BEMAN_EXECUTION26_DELETE("the object requires a query(get_allocator_t) overload");
        template <typename Object>
            requires (not requires(Object const& object, get_allocator_t const& tag)
            {
                { object.query(tag) } noexcept;
            })
        auto operator()(Object&&) const
            = BEMAN_EXECUTION26_DELETE("the query(get_allocator_t) overload needs to be noexcept");
        template <typename Object>
            requires (not requires(Object const& object, get_allocator_t const& tag)
            {
                { object.query(tag) } noexcept -> ::beman::execution26::detail::simple_allocator<>;
            })
        auto operator()(Object&&) const
            = BEMAN_EXECUTION26_DELETE("the query(get_allocator_t) overload needs to be noexcept");

        template <typename Object>
        auto operator()(Object&& object) const
        {
            return ::std::as_const(object).query(*this);
        }

        constexpr auto query(::beman::execution26::forwarding_query_t const&) const noexcept -> bool
        {
            return true;
        }
    };

    inline constexpr get_allocator_t get_allocator{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/get_env.hpp                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_ENV
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_ENV


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct get_env_t
    {
        template <typename Object>
            requires(
                not requires(Object&& object) { ::std::as_const(object).get_env(); }
                || ::beman::execution26::detail::queryable<std::remove_cvref_t<decltype(::std::declval<::std::remove_cvref_t<Object> const&>().get_env())>>
            )
        auto operator()(Object&& object) const -> decltype(auto)
        {
            if constexpr (requires{ ::std::as_const(object).get_env(); })
            {
                static_assert(noexcept(::std::as_const(object).get_env()),
                              "get_env requires the xpression to be noexcept");
                static_assert(::beman::execution26::detail::queryable<std::remove_cvref_t<decltype(::std::as_const(object).get_env())>>,
                              "get_env requires the result type to be destructible");
                return ::std::as_const(object).get_env();
            }
            else
            {
                return ::beman::execution26::empty_env{};
            }
        }
    };

    inline constexpr get_env_t get_env{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/get_domain.hpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_DOMAIN
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_DOMAIN


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct get_domain_t
    {
        template <typename Object>
            requires (not requires(Object&& object, get_domain_t const& tag)
            {
                ::std::forward<Object>(object).query(tag);
            })
            && (not requires(Object&& object, get_domain_t const& tag)
            {
                ::std::as_const(object).query(tag);
            })
        auto operator()(Object&&) const noexcept
            = BEMAN_EXECUTION26_DELETE("object neeeds a query(get_domain_t) overload");
        template <typename Object>
            requires (not requires(Object&& object, get_domain_t const& tag)
            {
                ::std::as_const(object).query(tag);
            })
        auto operator()(Object&&) const noexcept
            = BEMAN_EXECUTION26_DELETE("query(get_domain_t) overload needs to be const");
        template <typename Object>
            requires (not requires(Object&& object, get_domain_t const& tag)
            {
                { ::std::as_const(object).query(tag) } noexcept;
            })
        auto operator()(Object&&) const noexcept
            = BEMAN_EXECUTION26_DELETE("query(get_domain_t) overload needs to be noexcept");

        template <typename Object>
        constexpr auto operator()(Object&& object) const noexcept
        {
            return ::std::as_const(object).query(*this);
        }
        constexpr auto query(::beman::execution26::forwarding_query_t const&) const noexcept -> bool
        {
            return true;
        }
    };

    inline constexpr get_domain_t get_domain{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/get_scheduler.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_SCHEDULER


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct get_scheduler_t
        : ::beman::execution26::forwarding_query_t
    {
        template <typename Env>
            requires requires(get_scheduler_t const& self, Env&& env)
            {
                ::std::as_const(env).query(self); 

            }
        auto operator()(Env&& env) const noexcept
        {
            static_assert(noexcept(::std::as_const(env).query(*this)));
            //-dk:TODO mandate that the result is a scheduler
            // static_assert(::beman::execution26::scheduler<
            //     decltype(::std::as_const(env).query(*this))
            // >)
            return ::std::as_const(env).query(*this); 
        }
    };

    inline constexpr get_scheduler_t get_scheduler{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/set_value.hpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SET_VALUE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SET_VALUE


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct set_value_t
    {
        template <typename Receiver, typename... Args>
        auto operator()(Receiver&, Args&&...) const -> void
            = BEMAN_EXECUTION26_DELETE("set_value requires the receiver to be passed as non-const rvalue");
        template <typename Receiver, typename... Args>
        auto operator()(Receiver const&&, Args&&...) const -> void
            = BEMAN_EXECUTION26_DELETE("set_value requires the receiver to be passed as non-const rvalue");
        template <typename Receiver, typename... Args>
        auto operator()(Receiver&&, Args&&...) const -> void
            requires (not requires(Receiver&& receiver, Args&&... args)
            {
                ::std::forward<Receiver>(receiver).set_value(::std::forward<Args>(args)...);
            })
            = BEMAN_EXECUTION26_DELETE("set_value requires a suitable member overload on the receiver");
        template <typename Receiver, typename... Args>
            requires (not noexcept(::std::declval<Receiver>().set_value(::std::declval<Args>()...)))
        auto operator()(Receiver&&, Args&&...) const -> void
            = BEMAN_EXECUTION26_DELETE("the call to receiver.set_value(args...) has to be noexcept");

        template <typename Receiver, typename... Args>
        auto operator()(Receiver&& receiver, Args&&... args) const noexcept -> void
        {
            ::std::forward<Receiver>(receiver).set_value(::std::forward<Args>(args)...);
        }
    };
    inline constexpr set_value_t set_value{};
}

// ----------------------------------------------------------------------------

#endif
// inclue/beman/execution26/detail/set_error.hpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SET_ERROR
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SET_ERROR


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct set_error_t
    {
        template <typename Receiver, typename Error>
        auto operator()(Receiver&, Error&&) const -> void
            = BEMAN_EXECUTION26_DELETE("set_error requires the receiver to be passed as non-const rvalue");
        template <typename Receiver, typename Error>
        auto operator()(Receiver const&&, Error&&) const -> void
            = BEMAN_EXECUTION26_DELETE("set_error requires the receiver to be passed as non-const rvalue");
        template <typename Receiver, typename Error>
        auto operator()(Receiver&&, Error&&) const -> void
            requires (not requires(Receiver&& receiver, Error&& error)
            {
                ::std::forward<Receiver>(receiver).set_error(::std::forward<Error>(error));
            })
            = BEMAN_EXECUTION26_DELETE("set_error requires a suitable member overload on the receiver");
        template <typename Receiver, typename Error>
            requires (not noexcept(::std::declval<Receiver>().set_error(::std::declval<Error>())))
        auto operator()(Receiver&&, Error&&) const -> void
            = BEMAN_EXECUTION26_DELETE("the call to receiver.set_error(error) has to be noexcept");


        template <typename Receiver, typename Error>
        auto operator()(Receiver&& receiver, Error&& error) const noexcept -> void
        {
            ::std::forward<Receiver>(receiver).set_error(::std::forward<Error>(error));
        }
    };
    
    inline constexpr set_error_t set_error{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/set_stopped.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SET_STOPPED
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SET_STOPPED


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct set_stopped_t
    {
        template <typename Receiver>
        auto operator()(Receiver&) const -> void
            = BEMAN_EXECUTION26_DELETE("set_stopped requires the receiver to be passed as non-const rvalue");
        template <typename Receiver>
        auto operator()(Receiver const&&) const -> void
            = BEMAN_EXECUTION26_DELETE("set_stopped requires the receiver to be passed as non-const rvalue");
        template <typename Receiver>
        auto operator()(Receiver&&) const -> void
            requires (not requires(Receiver&& receiver)
            {
                ::std::forward<Receiver>(receiver).set_stopped();
            })
            = BEMAN_EXECUTION26_DELETE("set_stopped requires a suitable member overload on the receiver");
        template <typename Receiver>
            requires (not noexcept(::std::declval<Receiver>().set_stopped()))
        auto operator()(Receiver&&) const -> void
            = BEMAN_EXECUTION26_DELETE("the call to receiver.set_stopped() has to be noexcept");

        template <typename Receiver>
        auto operator()(Receiver&& receiver) const noexcept -> void
        {
            ::std::forward<Receiver>(receiver).set_stopped();
        }
    };

    inline constexpr set_stopped_t set_stopped{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/start.hpp                         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_START
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_START


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct start_t
    {
        template <typename State>
        auto operator()(State&&) const -> void
            = BEMAN_EXECUTION26_DELETE("start(obj) requires an lvalue argument");
        template <typename State>
            requires (not requires(State& state){ state.start(); })
        auto operator()(State&) const -> void
            = BEMAN_EXECUTION26_DELETE("state needs to have a start() member");
        template <typename State>
            requires (not requires(State const& state){ state.start(); })
        auto operator()(State const&) const -> void
            = BEMAN_EXECUTION26_DELETE("state needs to have a start() member");

        template <typename State>
            requires (not requires(State& state){ { state.start() } noexcept; })
        auto operator()(State&) const -> void
            = BEMAN_EXECUTION26_DELETE("state start() member has to be noexcept");
        template <typename State>
            requires (not requires(State const& state){ { state.start()} noexcept; })
        auto operator()(State const&) const -> void
            = BEMAN_EXECUTION26_DELETE("state start() member has to be noexcept");

        template <typename State>
        auto operator()(State const& state) const noexcept -> void
        {
            state.start();
        }
        template <typename State>
        auto operator()(State& state) const noexcept -> void
        {
            state.start();
        }
    };

    inline constexpr start_t start{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/meta_combine.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_META_COMBINE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_META_COMBINE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail::meta::detail
{
    template <typename...> struct combine;

    template <template <typename...> class L0, typename... T0>
    struct combine<L0<T0...>>
    {
        using type = L0<T0...>;
    };
    template <template <typename...> class L0, typename... T0,
              template <typename...> class L1, typename... T1,
              typename... L>
    struct combine<L0<T0...>, L1<T1...>, L...>
    {
        using type = typename combine<L0<T0..., T1...>, L...>::type;
    };
}

namespace beman::execution26::detail::meta
{
    template <typename... L>
    using combine
        = ::beman::execution26::detail::meta::detail::combine<L...>::type;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/meta_transform.hpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_META_TRANSFORM
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_META_TRANSFORM


// ----------------------------------------------------------------------------

namespace beman::execution26::detail::meta::detail
{
    template <template <typename> class Transform, typename List> struct transform;

    template <template <typename> class Transform,
              template <typename... T> class List,
              typename... T>
    struct transform<Transform, List<T...>>
    {
        using type = List<Transform<T>...>;
    };
}

namespace beman::execution26::detail::meta
{
    template <template <typename> class Transform, typename List>
    using transform = typename
        ::beman::execution26::detail::meta::detail::transform<Transform, List>
        ::type;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/meta_unique.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_META_UNIQUE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_META_UNIQUE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail::meta::detail
{
    template <typename, typename> struct make_unique;
    template <typename> struct unique;

    template <template <typename...> class List, typename... R>
    struct make_unique<List<R...>, List<>>
    {
        using type = List<R...>;
    };

    template <template <typename...> class List, typename... R, typename H, typename... T>
    struct make_unique<List<R...>, List<H, T...>>
    {
        using type = typename ::beman::execution26::detail::meta::detail::make_unique<
            ::std::conditional_t<
                ::beman::execution26::detail::meta::contains<H, R...>,
                List<R...>,
                List<R..., H>
            >,
            List<T...>
        >::type;
    };

    template <template <typename...> class List, typename... T>
    struct unique<List<T...>>
    {
        using type = typename ::beman::execution26::detail::meta::detail::make_unique<
            List<>,
            List<T...>
        >::type;
    };
}

namespace beman::execution26::detail::meta
{
    template <typename T>
    using unique
        = ::beman::execution26::detail::meta::detail::unique<T>::type;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/tag_of_t.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_TAG_OF
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_TAG_OF


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    template <typename Sender>
    using tag_of_t = typename decltype(
            ::beman::execution26::detail::get_sender_meta(::std::declval<Sender&&>())
        )::tag_type;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/nothrow_callable.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_NOTHROW_CALLABLE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_NOTHROW_CALLABLE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Fun, typename... Args>
    concept nothrow_callable
        =  ::beman::execution26::detail::callable<Fun, Args...>
        && requires(Fun&& fun, Args&&... args)
        {
            { ::std::forward<Fun>(fun)(::std::forward<Args>(args)...) } noexcept;
        }
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/fwd_env.hpp                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_FWD_ENV
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_FWD_ENV


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Env>
    class fwd_env
    {
    private:
        Env env;
    
    public:
        fwd_env(Env&& env): env(::std::forward<Env>(env)) {}

        template <typename Query, typename... Args>
            requires (not ::beman::execution26::forwarding_query(::std::remove_cvref_t<Query>()))
        constexpr auto query(Query&& q, Args&&... args) const
            = BEMAN_EXECUTION26_DELETE("the used query is not forwardable");

        template <typename Query, typename... Args>
            requires (::beman::execution26::forwarding_query(::std::remove_cvref_t<Query>()))
            && requires(Env const& env, Query&& q, Args&&... args)
            {
                env.query(q, ::std::forward<Args>(args)...);
            }
        constexpr auto query(Query&& q, Args&&... args) const
        {
            return env.query(q, ::std::forward<Args>(args)...);
        }
    };
    template <typename Env>
    fwd_env(Env&&) -> fwd_env<Env>;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/meta_filter.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_META_FILTER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_META_FILTER


// ----------------------------------------------------------------------------

namespace beman::execution26::detail::meta::detail
{
    template <template <typename> class, typename> struct filter;

    template <template <typename> class Predicate,
              template <typename...> class List>
    struct filter<Predicate, List<>>
    {
        using type = List<>;
    };

    template <template <typename> class Predicate,
              template <typename...> class List,
              typename H, typename... T>
    struct filter<Predicate, List<H, T...>>
    {
        using tail = typename beman::execution26::detail::meta::detail::filter<Predicate, List<T...>>::type;
        using type = ::std::conditional_t<
            Predicate<H>::value,
            ::beman::execution26::detail::meta::prepend<H, tail>,
            tail
        >;
    };
}

namespace beman::execution26::detail::meta
{
    template <template <typename> class Predicate, typename List>
    using filter = ::beman::execution26::detail::meta::detail::filter<Predicate, List>::type;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/context_base.hpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_CONTEXT_BASE
#define INCLUDED_BEMAN_NET29_DETAIL_CONTEXT_BASE


// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    struct context_base;
}

// ----------------------------------------------------------------------------

struct beman::net29::detail::context_base
{
    struct task
    {
        task* next;
        virtual auto complete() -> void = 0;
    };

    using accept_operation = ::beman::net29::detail::io_operation<
        ::std::tuple<::beman::net29::detail::endpoint,
                     ::socklen_t,
                     ::std::optional<::beman::net29::detail::socket_id>
                     >
        >;
    using connect_operation = ::beman::net29::detail::io_operation<
        ::std::tuple<::beman::net29::detail::endpoint>
        >;
    using receive_operation = ::beman::net29::detail::io_operation<
        ::std::tuple<::msghdr, int, ::std::size_t>
        >;
    using send_operation = ::beman::net29::detail::io_operation<
        ::std::tuple<::msghdr, int, ::std::size_t>
        >;
    using resume_after_operation = ::beman::net29::detail::io_operation<
        ::std::tuple<::std::chrono::system_clock::time_point, ::timeval>
        >;
    using resume_at_operation = ::beman::net29::detail::io_operation<
        ::std::tuple<::std::chrono::system_clock::time_point, ::timeval>
        >;

    virtual ~context_base() = default;
    virtual auto make_socket(int) -> ::beman::net29::detail::socket_id = 0;
    virtual auto make_socket(int, int, int, ::std::error_code&) -> ::beman::net29::detail::socket_id = 0;
    virtual auto release(::beman::net29::detail::socket_id, ::std::error_code&) -> void = 0;
    virtual auto native_handle(::beman::net29::detail::socket_id) -> ::beman::net29::detail::native_handle_type = 0;
    virtual auto set_option(::beman::net29::detail::socket_id, int, int, void const*, ::socklen_t, ::std::error_code&) -> void = 0;
    virtual auto bind(::beman::net29::detail::socket_id, ::beman::net29::detail::endpoint const&, ::std::error_code&) -> void = 0;
    virtual auto listen(::beman::net29::detail::socket_id, int, ::std::error_code&) -> void = 0;

    virtual auto run_one() -> ::std::size_t = 0;

    virtual auto cancel(::beman::net29::detail::io_base*, ::beman::net29::detail::io_base*) -> void = 0;
    virtual auto schedule(::beman::net29::detail::context_base::task*) -> void = 0;
    virtual auto accept(::beman::net29::detail::context_base::accept_operation*)
        -> ::beman::net29::detail::submit_result = 0;
    virtual auto connect(::beman::net29::detail::context_base::connect_operation*)
        -> ::beman::net29::detail::submit_result = 0;
    virtual auto receive(::beman::net29::detail::context_base::receive_operation*)
        -> ::beman::net29::detail::submit_result = 0;
    virtual auto send(::beman::net29::detail::context_base::send_operation*)
        -> ::beman::net29::detail::submit_result = 0;
    virtual auto resume_at(::beman::net29::detail::context_base::resume_at_operation*)
        -> ::beman::net29::detail::submit_result = 0;
};

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/stoppable_source.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_STOPPABLE_SOURCE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_STOPPABLE_SOURCE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template<typename Source>
    concept stoppable_source
        =   requires(Source& source, Source const& csource)
            {
                { csource.get_token() } -> ::beman::execution26::stoppable_token;
                { csource.stop_possible() } noexcept -> ::std::same_as<bool>;
                { csource.stop_requested() } noexcept -> ::std::same_as<bool>;
                { source.request_stop() } -> ::std::same_as<bool>;
            }
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/get_stop_token.hpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_STOP_TOKEN
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_STOP_TOKEN


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct get_stop_token_t
    {
        template <typename Object>
            requires requires(Object&& object, get_stop_token_t const& tag)
            {
                { ::std::as_const(object).query(tag) } noexcept -> ::beman::execution26::stoppable_token;
            }
        auto operator()(Object&& object) const noexcept
        {
            return ::std::as_const(object).query(*this);
        }

        template <typename Object>
        auto operator()(Object&&) const noexcept -> ::beman::execution26::never_stop_token
        {
            return {};
        }

        constexpr auto query(::beman::execution26::forwarding_query_t const&) const noexcept -> bool
        {
            return true;
        }
    };

    inline constexpr get_stop_token_t get_stop_token{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/completion_signature.hpp          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_SIGNATURE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_SIGNATURE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename> struct is_set_error : ::std::false_type {};
    template <typename Error>
    struct is_set_error<::beman::execution26::set_error_t(Error)>
        : ::std::true_type
    {
    };

    template <typename> struct is_set_stopped: ::std::false_type {};
    template <>
    struct is_set_error<::beman::execution26::set_stopped_t()>
        : ::std::true_type
    {
    };

    template <typename> struct is_set_value: ::std::false_type {};
    template <typename... Args>
    struct is_set_value<::beman::execution26::set_value_t(Args...)>
        : ::std::true_type
    {
    };

    template <typename Fun>
    concept completion_signature
        =  ::beman::execution26::detail::is_set_error<Fun>::value
        || ::beman::execution26::detail::is_set_stopped<Fun>::value
        || ::beman::execution26::detail::is_set_value<Fun>::value
        ;
}


// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/receiver.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_RECEIVER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_RECEIVER


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct receiver_t {};

    template <typename Rcvr>
    concept receiver
        =  ::std::derived_from<typename ::std::remove_cvref_t<Rcvr>::receiver_concept,
                             ::beman::execution26::receiver_t>
        && requires(::std::remove_cvref_t<Rcvr> const& rcvr)
            {
                { ::beman::execution26::get_env(rcvr) } -> ::beman::execution26::detail::queryable;
            }
        && ::std::move_constructible<::std::remove_cvref_t<Rcvr>>
        && ::std::constructible_from<::std::remove_cvref_t<Rcvr>, Rcvr>
        && (not ::std::is_final_v<::std::remove_cvref_t<Rcvr>>)
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/completion_tag.hpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_TAG
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_TAG


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Tag>
    concept completion_tag
        =  ::std::same_as<Tag, ::beman::execution26::set_error_t>
        || ::std::same_as<Tag, ::beman::execution26::set_stopped_t>
        || ::std::same_as<Tag, ::beman::execution26::set_value_t>
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/functional.hpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_FUNCTIONAL
#define INCLUDED_BEMAN_EXECUTION26_FUNCTIONAL

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/sender.hpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct sender_t {};
}
namespace beman::execution26::detail
{
    template <typename Sender>
    concept is_sender
        = ::std::derived_from<typename Sender::sender_concept, ::beman::execution26::sender_t>
        ;

    template <typename Sender>
    concept enable_sender
        =  ::beman::execution26::detail::is_sender<Sender>
        //-dk:TODO || ::beman::execution26::detail::is_awaitable<Sender,
        //    ::beman::Execution::detail::env_promise<::beman::Execution::empty_env>>
        ;
}
namespace beman::execution26
{
    template <typename Sender>
    concept sender
        =  ::beman::execution26::detail::enable_sender<::std::remove_cvref_t<Sender>>
        && requires(::std::remove_cvref_t<Sender> const& sndr)
        {
            { ::beman::execution26::get_env(sndr) } -> ::beman::execution26::detail::queryable;
        }
        && ::std::move_constructible<::std::remove_cvref_t<Sender>>
        && ::std::constructible_from<::std::remove_cvref_t<Sender>, Sender>
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/default_impls.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_DEFAULT_IMPLS
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_DEFAULT_IMPLS



// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    struct default_impls
    {
        static constexpr auto get_attrs
            = [](auto const&, auto const&... child) noexcept -> decltype(auto)
            {
                if constexpr (1 == sizeof...(child))
                    return (::beman::execution26::detail::fwd_env(
                        ::beman::execution26::get_env(child)
                    ), ...);
                else
                    return ::beman::execution26::empty_env{};
            };
        static constexpr auto get_env
            = [](auto, auto&, auto const& receiver) noexcept -> decltype(auto)
            {
                return ::beman::execution26::detail::fwd_env(
                    ::beman::execution26::get_env(receiver)
                );
            };
        static constexpr auto get_state
            = []<typename Sender, typename Receiver>(Sender&& sender, Receiver&) noexcept -> decltype(auto)
            {
                auto&& decompose = ::beman::execution26::detail::get_sender_data(::std::forward<Sender>(sender));
                return ::beman::execution26::detail::forward_like<Sender>(decompose.data);
            };
        static constexpr auto start
            = [](auto&, auto&, auto&... ops) noexcept -> void
            {
                (::beman::execution26::start(ops), ...);
            };
        static constexpr auto complete
            = []<typename Index, typename Receiver, typename Tag, typename... Args>(
                Index, auto&, Receiver& receiver, Tag, Args&&... args) noexcept -> void
                requires ::beman::execution26::detail::callable<Tag, Receiver, Args...>
            {
                static_assert(Index::value == 0);
                Tag()(::std::move(receiver), ::std::forward<Args>(args)...);
            };
    };
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/env_of_t.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_ENV_OF
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_ENV_OF


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    template <typename T>
    using env_of_t = decltype(::beman::execution26::get_env(::std::declval<T>()));
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/variant_or_empty.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_VARIANT_OR_EMPTY
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_VARIANT_OR_EMPTY


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    struct empty_variant
    {
        empty_variant() = delete;
    };

    template <typename... T> struct variant_or_empty_helper;

    template <>
    struct variant_or_empty_helper<>
    {
        using type = ::beman::execution26::detail::empty_variant;
    };

    template <typename... T>
    struct variant_or_empty_helper
    {
        using type = ::beman::execution26::detail::meta::unique<
            ::std::variant<::std::decay_t<T>...>
        >;
    };

    template <typename... T>
    using variant_or_empty
        = typename ::beman::execution26::detail::variant_or_empty_helper<T...>::type;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/poll_context.hpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_POLL_CONTEXT
#define INCLUDED_BEMAN_NET29_DETAIL_POLL_CONTEXT

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    struct poll_record;
    struct poll_context;
}

// ----------------------------------------------------------------------------

struct beman::net29::detail::poll_record final
{
    poll_record(::beman::net29::detail::native_handle_type h): handle(h) {}
    ::beman::net29::detail::native_handle_type handle;
    bool                                       blocking{true};
};

// ----------------------------------------------------------------------------

struct beman::net29::detail::poll_context final
    : ::beman::net29::detail::context_base
{
    using time_t = ::std::chrono::system_clock::time_point;
    using timer_node_t = ::beman::net29::detail::context_base::resume_at_operation;
    struct get_time { auto operator()(auto* t) const -> time_t { return ::std::get<0>(*t); } };
    using timer_priority_t
        = ::beman::net29::detail::sorted_list< timer_node_t, ::std::less<>, get_time>;
    ::beman::net29::detail::container<::beman::net29::detail::poll_record> d_sockets;
    ::std::vector<::pollfd>                         d_poll;
    ::std::vector<::beman::net29::detail::io_base*> d_outstanding;
    timer_priority_t                                d_timeouts;
    ::beman::net29::detail::context_base::task*     d_tasks{};

    auto make_socket(int fd) -> ::beman::net29::detail::socket_id override final
    {
        return this->d_sockets.insert(fd);
    }
    auto make_socket(int d, int t, int p, ::std::error_code& error)
        -> ::beman::net29::detail::socket_id override final
    {
        int fd(::socket(d, t, p));
        if (fd < 0)
        {
            error = ::std::error_code(errno, ::std::system_category());
            return ::beman::net29::detail::socket_id::invalid;
        }
        return this->make_socket(fd);
    }
    auto release(::beman::net29::detail::socket_id id, ::std::error_code& error) -> void override final
    {
        ::beman::net29::detail::native_handle_type handle(this->d_sockets[id].handle);
        this->d_sockets.erase(id);
        if (::close(handle) < 0)
        {
            error = ::std::error_code(errno, ::std::system_category());
        }
    }
    auto native_handle(::beman::net29::detail::socket_id id) -> ::beman::net29::detail::native_handle_type override final
    {
        return this->d_sockets[id].handle;
    }
    auto set_option(::beman::net29::detail::socket_id id,
                     int level,
                     int name,
                     void const* data,
                     ::socklen_t size,
                     ::std::error_code& error) -> void override final
    {
        if (::setsockopt(this->native_handle(id), level, name, data, size) < 0)
        {
            error = ::std::error_code(errno, ::std::system_category());
        }
    }
    auto bind(::beman::net29::detail::socket_id id,
               ::beman::net29::detail::endpoint const& endpoint,
               ::std::error_code& error) -> void override final
    {
        if (::bind(this->native_handle(id), endpoint.data(), endpoint.size()) < 0)
        {
            error = ::std::error_code(errno, ::std::system_category());
        }
    }
    auto listen(::beman::net29::detail::socket_id id, int no, ::std::error_code& error) -> void override final
    {
        if (::listen(this->native_handle(id), no) < 0)
        {
            error = ::std::error_code(errno, ::std::system_category());
        }
    }

    auto process_task() -> ::std::size_t
    {
        if (this->d_tasks)
        {
            auto* task{this->d_tasks};
            this->d_tasks = task->next;
            task->complete();
            return 1u;
        }
        return 0u;
    }
    auto process_timeout(auto const& now) -> ::std::size_t
    {
        if (!this->d_timeouts.empty() && ::std::get<0>(*this->d_timeouts.front()) <= now)
        {
            this->d_timeouts.pop_front()->complete();
            return 1u;
        }
        return 0u;
    }
    auto remove_outstanding(::std::size_t i)
    {
        if (i + 1u != this->d_poll.size())
        {
            this->d_poll[i] = this->d_poll.back();
            this->d_outstanding[i] = this->d_outstanding.back();
        }
        this->d_poll.pop_back();
        this->d_outstanding.pop_back();
    }
    auto to_milliseconds(auto duration) -> int
    {
        return ::std::chrono::duration_cast<::std::chrono::milliseconds>(duration).count();
    }
    auto run_one() -> ::std::size_t override final
    {
        auto now{::std::chrono::system_clock::now()};
        if (0u < this->process_timeout(now)
            || 0 < this->process_task()
            )
        {
            return 1u;
        }
        if (this->d_poll.empty() && this->d_timeouts.empty())
        {
            return ::std::size_t{};
        }
        while (true)
        {
            auto next_time{this->d_timeouts.value_or(now)};
            int timeout{now == next_time? -1: this->to_milliseconds(next_time - now)};
            int rc(::poll(this->d_poll.data(), this->d_poll.size(), timeout));
            if (rc < 0)
            {
                switch (errno)
                {
                default:
                    return ::std::size_t();
                case EINTR:
                case EAGAIN:
                    break;
                }
            }
            else
            {
                for (::std::size_t i(this->d_poll.size()); 0 < i--; )
                {
                    if (this->d_poll[i].revents & (this->d_poll[i].events | POLLERR))
                    {
                        ::beman::net29::detail::io_base* completion = this->d_outstanding[i];
                        this->remove_outstanding(i);
                        completion->work(*this, completion);
                        return ::std::size_t(1);
                    }
                }
                if (0u < this->process_timeout(::std::chrono::system_clock::now()))
                {
                    return 1u;
                }
            }
        }
        return ::std::size_t{};
    }
    auto wakeup() -> void
    {
        //-dk:TODO wake-up polling thread
    }

    auto add_outstanding(::beman::net29::detail::io_base* completion)
        -> ::beman::net29::detail::submit_result
    {
        auto id{completion->id};
        if (this->d_sockets[id].blocking
            || completion->work(*this, completion) == ::beman::net29::detail::submit_result::submit)
        {
            this->d_poll.emplace_back(::pollfd{this->native_handle(id), short(completion->event), short()});
            this->d_outstanding.emplace_back(completion);
            this->wakeup();
            return ::beman::net29::detail::submit_result::submit;
        }
        return ::beman::net29::detail::submit_result::ready;
    }

    auto cancel(::beman::net29::detail::io_base* cancel_op, ::beman::net29::detail::io_base* op) -> void override final
    {
        auto it(::std::find(this->d_outstanding.begin(), this->d_outstanding.end(), op));
        if (it != this->d_outstanding.end())
        {
            this->remove_outstanding(::std::distance(this->d_outstanding.begin(), it));
            op->cancel();
            cancel_op->cancel();
        }
        else if (this->d_timeouts.erase(op))
        {
            op->cancel();
            cancel_op->cancel();
        }
        else
        {
            std::cerr << "ERROR: poll_context::cancel(): entity not cancelled!\n";
        }
    }
    auto schedule(::beman::net29::detail::context_base::task* task) -> void override
    {
        task->next = this->d_tasks;
        this->d_tasks = task;
    }
    auto accept(::beman::net29::detail::context_base::accept_operation* completion)
        -> ::beman::net29::detail::submit_result override final
    {
        completion->work =
            [](::beman::net29::detail::context_base& ctxt,
               ::beman::net29::detail::io_base* comp)
            {
                auto id{comp->id};
                auto& completion(*static_cast<accept_operation*>(comp));

                while (true)
                {
                    int rc = ::accept(ctxt.native_handle(id), ::std::get<0>(completion).data(), &::std::get<1>(completion));
                    if (0 <= rc)
                    {
                        ::std::get<2>(completion) =  ctxt.make_socket(rc);
                        completion.complete();
                        return ::beman::net29::detail::submit_result::ready;
                    }
                    else
                    {
                        switch (errno)
                        {
                        default:
                            completion.error(::std::error_code(errno, ::std::system_category()));
                            return ::beman::net29::detail::submit_result::error;
                        case EINTR:
                            break;
                        case EWOULDBLOCK:
                            return ::beman::net29::detail::submit_result::submit;
                        }
                    }
                }
            };
        return this->add_outstanding(completion);
    }
    auto connect(::beman::net29::detail::context_base::connect_operation* op)
        -> ::beman::net29::detail::submit_result override
    {
        auto handle{this->native_handle(op->id)};
        auto const& endpoint(::std::get<0>(*op));
        if (-1 == ::fcntl(handle, F_SETFL, O_NONBLOCK))
        {
            op->error(::std::error_code(errno, ::std::system_category()));
            return ::beman::net29::detail::submit_result::error;
        }
        if (0 == ::connect(handle, endpoint.data(), endpoint.size()))
        {
            op->complete();
            return ::beman::net29::detail::submit_result::ready;
        }
        switch (errno)
        {
        default:
            op->error(::std::error_code(errno, ::std::system_category()));
            return ::beman::net29::detail::submit_result::error;
        case EINPROGRESS:
        case EINTR:
            break;
        }

        op->context = this;
        op->work = [](::beman::net29::detail::context_base& ctxt,
                      ::beman::net29::detail::io_base* op)
        {
            auto handle{ctxt.native_handle(op->id)};

            int error{};
            ::socklen_t len{sizeof(error)};
            if (-1 == ::getsockopt(handle, SOL_SOCKET, SO_ERROR, &error, &len))
            {
                op->error(::std::error_code(errno, ::std::system_category()));
                return ::beman::net29::detail::submit_result::error;
            }
            if (0 == error)
            {
                op->complete();
                return ::beman::net29::detail::submit_result::ready;
            }
            else
            {
                op->error(::std::error_code(error, ::std::system_category()));
                return ::beman::net29::detail::submit_result::error;
            }
        };

        return this->add_outstanding(op);
    } 
    auto receive(::beman::net29::detail::context_base::receive_operation* op)
        -> ::beman::net29::detail::submit_result override
    {
        op->context = this;
        op->work = [](::beman::net29::detail::context_base& ctxt,
                      ::beman::net29::detail::io_base* op)
        {
            auto& completion(*static_cast<receive_operation*>(op));
            while (true)
            {
                auto rc{::recvmsg(ctxt.native_handle(op->id),
                                  &::std::get<0>(completion),
                                  ::std::get<1>(completion))};
                if (0 <= rc)
                {
                    ::std::get<2>(completion) = rc;
                    completion.complete();
                    return ::beman::net29::detail::submit_result::ready;
                }
                else switch (errno)
                {
                default:
                    completion.error(::std::error_code(errno, ::std::system_category()));
                    return ::beman::net29::detail::submit_result::error;
                case ECONNRESET:
                case EPIPE:
                    ::std::get<2>(completion) = 0u;
                    completion.complete();
                    return ::beman::net29::detail::submit_result::ready;
                case EINTR:
                    break;
                case EWOULDBLOCK:
                    return ::beman::net29::detail::submit_result::submit;
                }
            }
        };
        return this->add_outstanding(op);
    }
    auto send(::beman::net29::detail::context_base::send_operation* op)
        -> ::beman::net29::detail::submit_result override
    {
        op->context = this;
        op->work = [](::beman::net29::detail::context_base& ctxt,
                      ::beman::net29::detail::io_base* op)
        {
            auto& completion(*static_cast<send_operation*>(op));

            while (true)
            {
                auto rc{::sendmsg(ctxt.native_handle(op->id),
                                    &::std::get<0>(completion),
                                    ::std::get<1>(completion))};
                if (0 <= rc)
                {
                    ::std::get<2>(completion) = rc;
                    completion.complete();
                    return ::beman::net29::detail::submit_result::ready;
                }
                else switch (errno)
                {
                default:
                    completion.error(::std::error_code(errno, ::std::system_category()));
                    return ::beman::net29::detail::submit_result::error;
                case ECONNRESET:
                case EPIPE:
                    ::std::get<2>(completion) = 0u;
                    completion.complete();
                    return ::beman::net29::detail::submit_result::ready;
                case EINTR:
                    break;
                case EWOULDBLOCK:
                    return ::beman::net29::detail::submit_result::submit;
                }
            }
        };
        return this->add_outstanding(op);
    }
    auto resume_at(::beman::net29::detail::context_base::resume_at_operation* op) -> ::beman::net29::detail::submit_result override
    {
        if (::std::chrono::system_clock::now() < ::std::get<0>(*op))
        {
            this->d_timeouts.insert(op);
            return ::beman::net29::detail::submit_result::submit;
        }
        else
        {
            op->complete();
            return ::beman::net29::detail::submit_result::ready;
        }
    }
};

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/unstoppable_token.hpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_UNSTOPPABLE_TOKEN
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_UNSTOPPABLE_TOKEN


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    template<typename Token>
    concept unstoppable_token
        =  ::beman::execution26::stoppable_token<Token>
        && requires()
            {
                requires ::std::bool_constant<not Token::stop_possible()>::value;
            }
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/completion_signatures.hpp         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_SIGNATURES
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_SIGNATURES


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    template <::beman::execution26::detail::completion_signature...>
    struct completion_signatures
    {
    };
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/valid_completion_for.hpp          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_VALID_COMPLETION_FOR
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_VALID_COMPLETION_FOR


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename, typename> struct valid_completion_for_aux;

    template <typename Rcvr, typename Tag, typename... Args>
    struct valid_completion_for_aux<Rcvr, Tag(*)(Args...)>
    {
        static auto test(Tag(*)(Args...)) -> void
            requires ::beman::execution26::detail::callable<Tag, ::std::remove_cvref_t<Rcvr>, Args...>
        {
        }
    };

    template <typename Signature, typename Rcvr>
    concept valid_completion_for
        = requires(Signature* signature)
        {
            #if 1
            valid_completion_for_aux<Rcvr, Signature*>::test(signature);
            #else
            // This definition crashes some versions of clang.
            []<typename Tag, typename... Args>(Tag(*)(Args...))
                requires ::beman::execution26::detail::callable<Tag, ::std::remove_cvref_t<Rcvr>, Args...>
            {
            }(signature);
            #endif
        }
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/schedule.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SCHEDULE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SCHEDULE


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct schedule_t
    {
        template <typename Scheduler>
            requires (not requires(Scheduler&& sched)
            {
                { ::std::forward<Scheduler>(sched).schedule() }
                    -> ::beman::execution26::sender;
            })
        auto operator()(Scheduler&& sched) const
            = BEMAN_EXECUTION26_DELETE("the scheduler needs a schedule() member returning a sender");

        template <typename Scheduler>
            requires requires(Scheduler&& sched)
            {
                { ::std::forward<Scheduler>(sched).schedule() }
                    -> ::beman::execution26::sender;
            }
        auto operator()(Scheduler&& sched) const
            noexcept(noexcept(std::forward<Scheduler>(sched).schedule()))
        {
            return std::forward<Scheduler>(sched).schedule();
        }
    };

    inline constexpr ::beman::execution26::schedule_t schedule{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/impls_for.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_IMPLS_FOR
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_IMPLS_FOR


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Tag>
    struct impls_for
        : ::beman::execution26::detail::default_impls
    {
    };
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/sender_adaptor_closure.hpp        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_ADAPTOR_CLOSURE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_ADAPTOR_CLOSURE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail::pipeable
{
    struct sender_adaptor_closure_base {};
}

namespace beman::execution26
{
    template <typename>
    struct sender_adaptor_closure
        : ::beman::execution26::detail::pipeable::sender_adaptor_closure_base
    {
    };
}

namespace beman::execution26::detail::pipeable
{
    template <::beman::execution26::sender Sender, typename Adaptor>
        requires (not ::beman::execution26::sender<Adaptor>)
        && ::std::derived_from<::std::decay_t<Adaptor>,
            ::beman::execution26::sender_adaptor_closure<::std::decay_t<Adaptor>>>
        && requires(Sender&& sender, Adaptor&& adaptor)
        {
            { adaptor(::std::forward<Sender>(sender)) } -> ::beman::execution26::sender;
        }
    auto operator| (Sender&& sender, Adaptor&& adaptor)
    {
        return adaptor(::std::forward<Sender>(sender));
    }
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/default_domain.hpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_DEFAULT_DOMAIN
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_DEFAULT_DOMAIN


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct default_domain
    {
        template <::beman::execution26::sender Sender,
                  ::beman::execution26::detail::queryable... Env>
            requires (sizeof...(Env) <= 1)
            && requires(Sender&& sender, Env&&... env)
            {
                ::beman::execution26::tag_of_t<Sender>().transform_sender(::std::forward<Sender>(sender), ::std::forward<Env>(env)...);
            }
        static constexpr auto transform_sender(Sender&& sender, Env&&... env)
            noexcept(noexcept(::beman::execution26::tag_of_t<Sender>().transform_sender(::std::forward<Sender>(sender), ::std::forward<Env>(env)...)))
            -> ::beman::execution26::sender decltype(auto)
        {
            return ::beman::execution26::tag_of_t<Sender>().transform_sender(::std::forward<Sender>(sender), ::std::forward<Env>(env)...);
        }

        template <::beman::execution26::sender Sender,
                  ::beman::execution26::detail::queryable... Env>
            requires (sizeof...(Env) <= 1)
            && (not requires(Sender&& sender, Env&&... env)
            {
                ::beman::execution26::tag_of_t<Sender>().transform_sender(::std::forward<Sender>(sender), ::std::forward<Env>(env)...);
            })
        static constexpr auto transform_sender(Sender&& sender, Env&&...)
            noexcept(noexcept(::std::forward<Sender>(sender)))
            -> ::beman::execution26::sender decltype(auto)
        {
            return ::std::forward<Sender>(sender);
        }

        template <::beman::execution26::sender Sender,
                  ::beman::execution26::detail::queryable Env>
            requires requires(Sender&& sender, Env&& env)
            {
                ::beman::execution26::tag_of_t<Sender>().transform_env(::std::forward<Sender>(sender), ::std::forward<Env>(env));
            }
        static constexpr auto transform_env(Sender&& sender, Env&& env) noexcept
            -> ::beman::execution26::detail::queryable decltype(auto)
        {
            return ::beman::execution26::tag_of_t<Sender>().transform_env(::std::forward<Sender>(sender), ::std::forward<Env>(env));
        }

        template <::beman::execution26::sender Sender,
                  ::beman::execution26::detail::queryable Env>
            requires (not requires(Sender&& sender, Env&& env)
            {
                ::beman::execution26::tag_of_t<Sender>().transform_env(::std::forward<Sender>(sender), ::std::forward<Env>(env));
            })
        static constexpr auto transform_env(Sender&&, Env&& env) noexcept
            -> ::beman::execution26::detail::queryable decltype(auto)
        {
            return static_cast<Env>(::std::forward<Env>(env));
        }

        template <typename Tag, ::beman::execution26::sender Sender, typename... Args>
            requires requires(Sender&& sender, Args&&... args)
            {
                Tag().apply_sender(::std::forward<Sender>(sender), ::std::forward<Args>(args)...);
            }
        static constexpr auto apply_sender(Tag, Sender&& sender, Args&&... args)
            noexcept(noexcept(Tag().apply_sender(::std::forward<Sender>(sender), ::std::forward<Args>(args)...)))
            -> decltype(auto)
        {
            return Tag().apply_sender(::std::forward<Sender>(sender), ::std::forward<Args>(args)...);
        }
    };
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/stop_token.hpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_STOP_TOKEN
#define INCLUDED_BEMAN_EXECUTION26_STOP_TOKEN


// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/valid_completion_signatures.hpp   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_VALID_COMPLETION_SIGNATURES
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_VALID_COMPLETION_SIGNATURES


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename>
    struct valid_completion_signatures_helper
        : ::std::false_type
    {
    };
    template <typename... Sigs>
    struct valid_completion_signatures_helper<::beman::execution26::completion_signatures<Sigs...>>
        : ::std::true_type
    {
    };

    template <typename Signatures>
    concept valid_completion_signatures
        = valid_completion_signatures_helper<Signatures>::value
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/almost_scheduler.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_ALMOST_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_ALMOST_SCHEDULER


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct scheduler_t {};
}

namespace beman::execution26::detail
{
    template <typename Scheduler>
    concept almost_scheduler
        = ::std::derived_from<typename ::std::remove_cvref_t<Scheduler>::scheduler_concept,
                              ::beman::execution26::scheduler_t>
        && ::beman::execution26::detail::queryable<Scheduler>
        && requires(Scheduler&& sched) {
            { ::beman::execution26::schedule(::std::forward<Scheduler>(sched)) }
                -> ::beman::execution26::sender;
        }
        && ::std::equality_comparable<::std::remove_cvref_t<Scheduler>>
        && ::std::copy_constructible<::std::remove_cvref_t<Scheduler>>
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/transform_sender.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_TRANSFORM_SENDER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_TRANSFORM_SENDER


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Domain, ::beman::execution26::sender Sender, typename... Env> 
        requires (sizeof...(Env) < 2)
        && requires(Domain dom, Sender&& sender, Env const& ... env)
        {
            dom.transform_sender(::std::forward<Sender>(sender), env...);
        }
        && (::std::same_as<::std::remove_cvref_t<Sender>,
            std::remove_cvref_t<decltype(::std::declval<Domain>().transform_sender(::std::declval<Sender>()))>>)
    constexpr auto transform_sender(Domain, Sender&& sender, Env const&...)
        noexcept
        -> ::beman::execution26::sender auto
    {
        return ::std::forward<Sender>(sender);
    }

    template <typename Domain, ::beman::execution26::sender Sender, typename... Env> 
        requires (sizeof...(Env) < 2)
        && requires(Domain dom, Sender&& sender, Env const& ... env)
        {
            dom.transform_sender(::std::forward<Sender>(sender), env...);
        }
        && (not ::std::same_as<::std::remove_cvref_t<Sender>,
            std::remove_cvref_t<decltype(::std::declval<Domain>().transform_sender(::std::declval<Sender>()))>>)
    constexpr auto transform_sender(Domain dom, Sender&& sender, Env const&... env)
        noexcept
        -> ::beman::execution26::sender decltype(auto)
    {
        return ::beman::execution26::detail::transform_sender(
            dom,
            dom.transform_sender(::std::forward<Sender>(sender), env...),
            env...);
    }

    template <typename Domain, ::beman::execution26::sender Sender, typename... Env> 
        requires (not requires(Domain dom, Sender&& sender, Env const& ... env)
        {
            dom.transform_sender(::std::forward<Sender>(sender), env...);
        })
        && ::std::same_as<::std::remove_cvref_t<Sender>,
            ::std::remove_cvref_t<decltype(
                ::beman::execution26::default_domain{}.transform_sender(::std::declval<Sender>(), ::std::declval<Env>()...)
            )>>
    constexpr auto transform_sender(Domain, Sender&& sender, Env const&...)
        noexcept(noexcept(::std::forward<Sender>(sender)))
        -> ::beman::execution26::sender auto
    {
        return sender;
    }

    template <typename Domain, ::beman::execution26::sender Sender, typename... Env> 
        requires (not requires(Domain dom, Sender&& sender, Env const& ... env)
        {
            dom.transform_sender(::std::forward<Sender>(sender), env...);
        })
        && (not ::std::same_as<::std::remove_cvref_t<Sender>,
            ::std::remove_cvref_t<decltype(
                ::beman::execution26::default_domain{}.transform_sender(::std::declval<Sender>(), ::std::declval<Env>()...)
            )>>)
    constexpr auto transform_sender(Domain dom, Sender&& sender, Env const&... env)
        noexcept(noexcept(
            ::beman::execution26::default_domain{}.transform_sender(::std::declval<Sender>(), ::std::declval<Env>()...)
        ))
        -> ::beman::execution26::sender decltype(auto)
    {
        (void)dom;
        return ::beman::execution26::detail::transform_sender(
            dom,
            ::beman::execution26::default_domain{}.transform_sender(::std::forward<Sender>(sender), env...),
            env...);
    }
}

namespace beman::execution26
{
    template <typename Domain, ::beman::execution26::sender Sender, typename... Env> 
        requires (sizeof...(Env) < 2)
        && requires(Domain dom, Sender&& sender, Env const& ... env)
        {
            dom.transform_sender(::std::forward<Sender>(sender), env...);
        }
        && (::std::same_as<::std::remove_cvref_t<Sender>,
            std::remove_cvref_t<decltype(::std::declval<Domain>().transform_sender(::std::declval<Sender>()))>>)
    constexpr auto transform_sender(Domain, Sender&& sender, Env const&...)
        noexcept
        -> ::beman::execution26::sender decltype(auto)
    {
        return ::std::forward<Sender>(sender);
    }

    template <typename Domain, ::beman::execution26::sender Sender, typename... Env> 
        requires (sizeof...(Env) < 2)
        && requires(Domain dom, Sender&& sender, Env const& ... env)
        {
            dom.transform_sender(::std::forward<Sender>(sender), env...);
        }
        && (not ::std::same_as<::std::remove_cvref_t<Sender>,
            std::remove_cvref_t<decltype(::std::declval<Domain>().transform_sender(::std::declval<Sender>()))>>)
    constexpr auto transform_sender(Domain dom, Sender&& sender, Env const&... env)
        noexcept
        -> ::beman::execution26::sender auto
    {
        return ::beman::execution26::detail::transform_sender(
            dom,
            dom.transform_sender(::std::forward<Sender>(sender), env...),
            env...);
    }

    template <typename Domain, ::beman::execution26::sender Sender, typename... Env> 
        requires (sizeof...(Env) < 2)
        && (not requires(Domain dom, Sender&& sender, Env const& ... env)
        {
            dom.transform_sender(::std::forward<Sender>(sender), env...);
        })
        && ::std::same_as<::std::remove_cvref_t<Sender>,
            ::std::remove_cvref_t<decltype(
                ::beman::execution26::default_domain{}.transform_sender(::std::declval<Sender>(), ::std::declval<Env>()...)
            )>>
    constexpr auto transform_sender(Domain, Sender&& sender, Env const&...)
        noexcept(noexcept(::std::forward<Sender>(sender)))
        -> ::beman::execution26::sender decltype(auto)
    {
        return ::std::forward<Sender>(sender);
    }

    template <typename Domain, ::beman::execution26::sender Sender, typename... Env> 
        requires (sizeof...(Env) < 2)
        && (not requires(Domain dom, Sender&& sender, Env const& ... env)
        {
            dom.transform_sender(::std::forward<Sender>(sender), env...);
        })
        && (not ::std::same_as<::std::remove_cvref_t<Sender>,
            ::std::remove_cvref_t<decltype(
                ::beman::execution26::default_domain{}.transform_sender(::std::declval<Sender>(), ::std::declval<Env>()...)
            )>>)
    constexpr auto transform_sender(Domain dom, Sender&& sender, Env const&... env)
        noexcept(noexcept(
            ::beman::execution26::detail::transform_sender(
                dom,
                ::beman::execution26::default_domain{}.transform_sender(::std::declval<Sender>(), ::std::declval<Env>()...),
                env...
            )
        ))
        -> ::beman::execution26::sender decltype(auto)
    {
        return ::beman::execution26::detail::transform_sender(
            dom,
            ::beman::execution26::default_domain{}.transform_sender(::std::forward<Sender>(sender), env...),
            env...);
    }
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/apply_sender.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_APPLY_SENDER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_APPLY_SENDER


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    template <typename Domain, typename Tag, ::beman::execution26::sender Sender, typename... Args>
        requires requires(Domain domain, Tag tag, Sender&& sender, Args&&... args)
        {
            domain.apply_sender(Tag(), ::std::forward<Sender>(sender), ::std::forward<Args>(args)...);
        }
    constexpr auto apply_sender(Domain domain, Tag, Sender&& sender, Args&&... args)
        noexcept(noexcept(domain.apply_sender(Tag(), ::std::forward<Sender>(sender), ::std::forward<Args>(args)...)))
        -> decltype(auto)
    {
        return domain.apply_sender(Tag(), ::std::forward<Sender>(sender), ::std::forward<Args>(args)...);
    }

    template <typename Domain, typename Tag, ::beman::execution26::sender Sender, typename... Args>
        requires (not requires(Domain domain, Tag tag, Sender&& sender, Args&&... args)
        {
            domain.apply_sender(Tag(), ::std::forward<Sender>(sender), ::std::forward<Args>(args)...);
        })
        && requires(Tag tag, Sender&& sender, Args&&... args)
        {
            beman::execution26::default_domain().apply_sender(Tag(), ::std::forward<Sender>(sender), ::std::forward<Args>(args)...);
        }
    constexpr auto apply_sender(Domain, Tag, Sender&& sender, Args&&... args)
        noexcept(noexcept(beman::execution26::default_domain().apply_sender(Tag(), ::std::forward<Sender>(sender), ::std::forward<Args>(args)...)))
        -> decltype(auto)
    {
        return beman::execution26::default_domain().apply_sender(Tag(), ::std::forward<Sender>(sender), ::std::forward<Args>(args)...);
    }

}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/sender_adaptor.hpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_ADAPTOR
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_ADAPTOR


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Adaptor, typename T0, typename... T>
    struct sender_adaptor
        : ::beman::execution26::detail::product_type<
            ::std::decay_t<Adaptor>, ::std::decay_t<T0>, ::std::decay_t<T>...>
        , ::beman::execution26::sender_adaptor_closure<sender_adaptor<Adaptor, T0, T...>>
    {
        template <::beman::execution26::sender Sender>
        static auto apply(Sender&& sender, auto&& self)
        {
            return [&self, &sender]<::std::size_t... I>(::std::index_sequence<I...>){
                return (self.template get<0>())(
                    ::std::forward<Sender>(sender),
                    self.template get<I + 1>()...
                );
            }(::std::make_index_sequence<sender_adaptor::size() - 1u>{});
        }
        template <::beman::execution26::sender Sender>
        auto operator()(Sender&& sender)
        {
            return apply(::std::forward<Sender>(sender), *this);
        }
        template <::beman::execution26::sender Sender>
        auto operator()(Sender&& sender) const
        {
            return apply(::std::forward<Sender>(sender), *this);
        }
    };
    template <typename... T>
    sender_adaptor(T&&...) -> sender_adaptor<T...>;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/state_type.hpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_STATE_TYPE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_STATE_TYPE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Sender, typename Receiver>
    using state_type = ::std::decay_t<
        ::beman::execution26::detail::call_result_t<
            decltype(::beman::execution26::detail::impls_for<
                ::beman::execution26::tag_of_t<Sender>
            >::get_state), Sender, Receiver&
        >
    >;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/stop_token.hpp                          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_STOP_TOKEN
#define INCLUDED_BEMAN_NET29_DETAIL_STOP_TOKEN


// ----------------------------------------------------------------------------

namespace beman::net29::detail::ex
{
    using ::beman::execution26::never_stop_token;
    using ::beman::execution26::inplace_stop_source;
    using ::beman::execution26::inplace_stop_token;
    using ::beman::execution26::stop_callback_for_t;
    using ::beman::execution26::detail::stoppable_callback_for;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/get_completion_scheduler.hpp      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_COMPLETION_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_COMPLETION_SCHEDULER


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    template <typename Tag>
    struct get_completion_scheduler_t;

    template <typename Tag>
    struct get_completion_scheduler_t
        : ::beman::execution26::forwarding_query_t
    {
        template <typename Env>
            requires (not requires(get_completion_scheduler_t const& self,
                                   ::std::remove_cvref_t<Env> const& env) {
                env.query(self);
            })
        auto operator()(Env&&) const noexcept
            = BEMAN_EXECUTION26_DELETE("The environment needs a query(get_completion_scheduler_t) member");

        template <typename Env>
            requires (requires(get_completion_scheduler_t const& self,
                                   ::std::remove_cvref_t<Env> const& env) {
                env.query(self);
            }
            && (not requires(get_completion_scheduler_t const& self,
                                   ::std::remove_cvref_t<Env> const& env) {
                { env.query(self) } noexcept;
            }))
        auto operator()(Env&&) const noexcept
            = BEMAN_EXECUTION26_DELETE("The environment's query(get_completion_scheduler_t) has to be noexcept");

        template <typename Env>
            requires (requires(get_completion_scheduler_t const& self,
                                   ::std::remove_cvref_t<Env> const& env) {
                env.query(self);
            }
            && requires(get_completion_scheduler_t const& self,
                                   ::std::remove_cvref_t<Env> const& env) {
                { env.query(self) } noexcept;
            }
            && (not requires(get_completion_scheduler_t const& self,
                                   get_completion_scheduler_t<::beman::execution26::set_value_t> const& value_self,
                                   ::std::remove_cvref_t<Env> const& env) {
                {
                    env.query(self)
                } noexcept -> ::beman::execution26::detail::almost_scheduler;
                {
                    ::beman::execution26::get_env(
                        ::beman::execution26::schedule(env.query(self))
                    ).query(value_self)
                } -> ::beman::execution26::detail::decayed_same_as<decltype(env.query(self))>;
            }))
        auto operator()(Env&&) const noexcept
            = BEMAN_EXECUTION26_DELETE("The environment's query(get_completion_scheduler_t) has to return a scheduler");

        template <typename Env>
            requires requires(get_completion_scheduler_t const& self,
                              get_completion_scheduler_t<::beman::execution26::set_value_t> const& value_self,
                              ::std::remove_cvref_t<Env> const& env) {
                {
                    env.query(self)
                } noexcept -> ::beman::execution26::detail::almost_scheduler;
                {
                    ::beman::execution26::get_env(
                        ::beman::execution26::schedule(env.query(self))
                    ).query(value_self)
                } -> ::beman::execution26::detail::decayed_same_as<decltype(env.query(self))>;
            }
        auto operator()(Env&& env) const noexcept
        {
            return ::std::as_const(env).query(*this); 
        }
    };

    template <::beman::execution26::detail::completion_tag Tag>
    inline constexpr get_completion_scheduler_t<Tag> get_completion_scheduler{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/gather_signatures.hpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_GATHER_SIGNATURES
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_GATHER_SIGNATURES


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename...> concept always_true = true;

    template <typename, typename> struct same_tag;
    template <typename Tag, typename R, typename... A>
    struct same_tag<Tag, R(A...)>
    {
        static constexpr bool value = ::std::same_as<Tag, R>;
    };
    template <typename Tag>
    struct bound_tag
    {
        template <typename T>
        using predicate = ::beman::execution26::detail::same_tag<Tag, T>;
    };

    template <typename, template <typename...> class>
    struct gather_signatures_apply;
    template <typename R, typename... A, template <typename...> class Transform>
        requires requires {
            typename ::beman::execution26::detail::indirect_meta_apply<::beman::execution26::detail::always_true<R>>
                ::template meta_apply<Transform, A...>
            ;
        }
    struct gather_signatures_apply<R(A...), Transform>
    {
        using type = ::beman::execution26::detail::indirect_meta_apply<::beman::execution26::detail::always_true<R>>
            ::template meta_apply<Transform, A...>
        ;
    };

    template <::beman::execution26::detail::valid_completion_signatures,
              template <typename...> class,
              template <typename...> class
            >
    struct gather_signatures_helper;

    template <typename... Signatures,
              template <typename...> class Tuple,
              template <typename...> class Variant>
        requires requires{
            requires always_true<
                typename ::beman::execution26::detail::gather_signatures_apply<Signatures, Tuple>::type...
            >;
        }
        && requires{
        typename ::beman::execution26::detail::indirect_meta_apply<
                always_true<
                    typename ::beman::execution26::detail::gather_signatures_apply<Signatures, Tuple>::type...
                >
            >
            ::template meta_apply<Variant,
                typename ::beman::execution26::detail::gather_signatures_apply<Signatures, Tuple>::type...
            >;
        }
    struct gather_signatures_helper<
        ::beman::execution26::completion_signatures<Signatures...>, Tuple, Variant>
    {
        using type = ::beman::execution26::detail::indirect_meta_apply<
                always_true<
                    typename ::beman::execution26::detail::gather_signatures_apply<Signatures, Tuple>::type...
                >
            >
            ::template meta_apply<Variant,
                typename ::beman::execution26::detail::gather_signatures_apply<Signatures, Tuple>::type...
            >
        ;
    };

    template <typename Tag,
              ::beman::execution26::detail::valid_completion_signatures signatures,
              template <typename...> class Tuple,
              template <typename...> class Variant>
        requires requires {
            typename ::beman::execution26::detail::gather_signatures_helper<
                ::beman::execution26::detail::meta::filter<
                    ::beman::execution26::detail::bound_tag<Tag>::template predicate,
                    signatures
                >,
                Tuple,
                Variant
            >::type;
        }
    using gather_signatures
        = ::beman::execution26::detail::gather_signatures_helper<
            ::beman::execution26::detail::meta::filter<
                ::beman::execution26::detail::bound_tag<Tag>::template predicate,
                signatures
            >,
            Tuple,
            Variant
        >::type;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/basic_state.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_STATE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_STATE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Sender, typename Receiver>
    struct basic_state
    {
        template <typename S> //-dk:TODO is that deviating from the spec?
        basic_state(S&& sender, Receiver&& receiver) noexcept(true)
            : receiver(::std::move(receiver))
            , state(::beman::execution26::detail::impls_for<
                    ::beman::execution26::tag_of_t<Sender>
                >::get_state(::std::forward<S>(sender), receiver))
        {
        }

        Receiver receiver;
        ::beman::execution26::detail::state_type<Sender, Receiver> state;
    };
    template <typename Sender, typename Receiver>
    basic_state(Sender&&, Receiver&&) -> basic_state<Sender&&, Receiver>;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/env_type.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_ENV_TYPE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_ENV_TYPE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Index, typename Sender, typename Receiver>
    using env_type = ::beman::execution26::detail::call_result_t<
        decltype(::beman::execution26::detail::impls_for<
            ::beman::execution26::tag_of_t<Sender>
        >::get_env),
        Index,
        ::beman::execution26::detail::state_type<Sender, Receiver>&,
        Receiver const&
    >;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/scheduler.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SCHEDULER


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    template <typename Scheduler>
    concept scheduler
        = ::beman::execution26::detail::almost_scheduler<Scheduler>
        && requires(Scheduler&& sched) {
            {
                ::beman::execution26::get_completion_scheduler<::beman::execution26::set_value_t>(
                    ::beman::execution26::get_env(
                        ::beman::execution26::schedule(
                            ::std::forward<Scheduler>(sched)
                        )
                    )
                )
            } -> ::beman::execution26::detail::decayed_same_as<Scheduler>;
        }
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/completion_domain.hpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_DOMAIN
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_DOMAIN


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    struct completion_domain_undefined {};
    template <typename, typename>
    struct completion_domain_merge
    {
    };
    template <typename T>
    struct completion_domain_merge<T, T>
    {
        using type = T;
    };
    template <typename T>
    struct completion_domain_merge<completion_domain_undefined, T>
    {
        using type = T;
    };
    template <typename T>
    struct completion_domain_merge<T, completion_domain_undefined>
    {
        using type = T;
    };
    template <>
    struct completion_domain_merge<completion_domain_undefined, completion_domain_undefined>
    {
        using type = completion_domain_undefined;
    };

    template <typename Default = ::beman::execution26::default_domain, typename Sender>
    constexpr auto completion_domain(Sender const& sender) noexcept
    {

        static_assert(::beman::execution26::sender<Sender>);
        auto get = [&sender]<typename Tag>(Tag) {
            if constexpr (requires{
                ::beman::execution26::get_domain(
                    ::beman::execution26::get_completion_scheduler<Tag>(
                        ::beman::execution26::get_env(sender)
                    )
                );
            })
            {
                return ::beman::execution26::get_domain(
                    ::beman::execution26::get_completion_scheduler<Tag>(
                        ::beman::execution26::get_env(sender)
                    )
                );
            }
            else
            {
                return completion_domain_undefined{};
            }
        };

        using type = typename completion_domain_merge<
                typename completion_domain_merge<
                    decltype(get(::beman::execution26::set_error)),
                    decltype(get(::beman::execution26::set_stopped))
                >::type,
                decltype(get(::beman::execution26::set_value))
            >::type;
        return ::std::conditional_t<
            ::std::same_as<type, completion_domain_undefined>,
            Default,
            type>();
    }
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/basic_receiver.hpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_RECEIVER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_RECEIVER


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Sender, typename Receiver, typename Index>
        requires ::beman::execution26::detail::valid_specialization<
            ::beman::execution26::detail::env_type, Index, Sender, Receiver
        >
    struct basic_receiver
    {
        using receiver_concept = ::beman::execution26::receiver_t;
        using tag_t = ::beman::execution26::tag_of_t<Sender>;
        using state_t = ::beman::execution26::detail::state_type<Sender, Receiver>;
        static constexpr const auto& complete = ::beman::execution26::detail::impls_for<tag_t>::complete;

        template <typename... Args>
        auto set_value(Args&&... args) && noexcept -> void
            requires ::beman::execution26::detail::callable<
                decltype(complete),
                Index,
                state_t&,
                Receiver&,
                ::beman::execution26::set_value_t,
                Args...>
        {
            this->complete(Index(), this->op->state, this->op->receiver,
                           ::beman::execution26::set_value_t(), ::std::forward<Args>(args)...);
        }

        template <typename Error>
        auto set_error(Error&& error) && noexcept -> void
            requires ::beman::execution26::detail::callable<
                decltype(complete),
                Index,
                state_t&,
                Receiver&,
                ::beman::execution26::set_error_t,
                Error>
        {
            this->complete(Index(), this->op->state, this->op->receiver,
                           ::beman::execution26::set_error_t(), ::std::forward<Error>(error));
        }

        auto set_stopped() && noexcept -> void
            requires ::beman::execution26::detail::callable<
                decltype(complete),
                Index,
                state_t&,
                Receiver&,
                ::beman::execution26::set_stopped_t>
        {
            this->complete(Index(), this->op->state, this->op->receiver,
                           ::beman::execution26::set_stopped_t());
        }

        auto get_env() const noexcept
            -> ::beman::execution26::detail::env_type<Index, Sender, Receiver>
        {
            return ::beman::execution26::detail::impls_for<tag_t>
                ::get_env(Index(), this->op->state, this->op->receiver);
        }

        ::beman::execution26::detail::basic_state<Sender, Receiver>* op{};
    };  
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/get_delegation_scheduler.hpp      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_DELEGATION_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_DELEGATION_SCHEDULER


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct get_delegation_scheduler_t
    {
        template <typename Env>
            requires requires(Env&& env, get_delegation_scheduler_t const& g)
            {
                { ::std::as_const(env).query(g) } noexcept
                    -> ::beman::execution26::scheduler;
            }
        auto operator()(Env&& env) const noexcept
        {
            return ::std::as_const(env).query(*this);
        }
        constexpr auto query(::beman::execution26::forwarding_query_t const&) const
            noexcept -> bool
        {
            return true;
        }
    };

    inline constexpr get_delegation_scheduler_t get_delegation_scheduler{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/get_domain_late.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_DOMAIN_LATE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_DOMAIN_LATE


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename T>
    concept not_void = not ::std::same_as<T, void>;

    template <typename Tag>
    struct get_domain_late_helper
    {
        template <typename Sender, typename Env>
        static constexpr auto get(Sender const& sender, Env const& env) noexcept
        {
            if constexpr (requires{ { ::beman::execution26::get_domain(::beman::execution26::get_env(sender)) } -> ::beman::execution26::detail::not_void; })
                return ::beman::execution26::get_domain(::beman::execution26::get_env(sender));
            else if constexpr (requires{ { ::beman::execution26::detail::completion_domain<void>(sender) } -> ::beman::execution26::detail::not_void; })
                return ::beman::execution26::detail::completion_domain<void>(sender);
            else if constexpr (requires{ { ::beman::execution26::get_domain(env) } -> ::beman::execution26::detail::not_void; })
                return ::beman::execution26::get_domain(env);
            else if constexpr (requires{ { ::beman::execution26::get_domain(::beman::execution26::get_scheduler(env)) } -> ::beman::execution26::detail::not_void; })
                return ::beman::execution26::get_domain(::beman::execution26::get_scheduler(env));
            else
                return ::beman::execution26::default_domain();
        }
    };
    template <typename Sender, typename Env>
    constexpr auto get_domain_late(Sender const& sender, Env const& env) noexcept
    {
        using tag_t = ::beman::execution26::tag_of_t<Sender>;
        return ::beman::execution26::detail::get_domain_late_helper<tag_t>::get(sender, env);
    }
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/get_domain_early.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_DOMAIN_EARLY
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_DOMAIN_EARLY


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Sender>
    constexpr auto get_domain_early(Sender const& sender) noexcept
    {
        if constexpr (requires{
                ::beman::execution26::get_domain(
                    ::beman::execution26::get_env(sender)
                );
            })
            return decltype(
                ::beman::execution26::get_domain(
                    ::beman::execution26::get_env(sender)
                )
            ){};
        else if constexpr (requires{
                ::beman::execution26::detail::completion_domain(sender);
            })
            return decltype(::beman::execution26::detail::completion_domain(sender)){};
        else
            return ::beman::execution26::default_domain{};
    }
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/run_loop.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_RUN_LOOP
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_RUN_LOOP



// ----------------------------------------------------------------------------

namespace beman::execution26
{
    class run_loop
    {
    private:
        struct scheduler;

        struct env
        {
            run_loop* loop;

            template <typename Completion>
            auto query(::beman::execution26::get_completion_scheduler_t<Completion> const&) const
                noexcept
                -> scheduler
            {
                return {this->loop};
            }
        };
        struct opstate_base
        {
            opstate_base* next{};
            virtual auto execute() noexcept -> void = 0;
        };
        template <typename Receiver>
        struct opstate
            : opstate_base
        {
            using operation_state_concept = ::beman::execution26::operation_state_t;

            run_loop* loop;
            Receiver  receiver;

            template <typename R>
            opstate(run_loop* loop, R&& receiver)
                : loop(loop)
                , receiver(::std::forward<Receiver>(receiver))
            {
            }
            opstate(opstate&&) = delete;
            auto start() & noexcept -> void
            {
                try
                {
                    this->loop->push_back(this);
                }
                catch(...)
                {
                    ::beman::execution26::set_error(
                        ::std::move(this->receiver),
                        ::std::current_exception());
                }
                
            }
            auto execute() noexcept -> void override
            {
                if (::beman::execution26::get_stop_token(
                        ::beman::execution26::get_env(this->receiver)
                    ).stop_requested())
                    ::beman::execution26::set_stopped(::std::move(this->receiver));
                else
                    ::beman::execution26::set_value(::std::move(this->receiver));
            }
        };
        struct sender
        {
            using sender_concept = ::beman::execution26::sender_t;
            using completion_signatures = ::beman::execution26::completion_signatures<
                    ::beman::execution26::set_value_t(),
                    ::beman::execution26::set_error_t(::std::exception_ptr),
                    ::beman::execution26::set_stopped_t()
                >;
            
            run_loop* loop;

            auto get_env() const noexcept -> env { return {this->loop}; }
            template <typename Receiver>
            auto connect(Receiver&& receiver) noexcept
                -> opstate<::std::decay_t<Receiver>>
            {
                return {this->loop, ::std::forward<Receiver>(receiver)};
            }
        };
        struct scheduler
        {
            using scheduler_concept = ::beman::execution26::scheduler_t;

            run_loop* loop;

            auto schedule() noexcept -> sender { return {this->loop}; }
            auto operator== (scheduler const&) const -> bool = default;
        };

        enum class state { starting, running, finishing };

        state                     current_state{state::starting};
        ::std::mutex              mutex{};
        ::std::condition_variable condition{};
        opstate_base*             front{};
        opstate_base*             back{};

        auto push_back(opstate_base* item) -> void
        {
            ::std::lock_guard guard(this->mutex);
            if (auto previous_back{::std::exchange(this->back, item)})
            {
                previous_back->next = item;
            }
            else
            {
                this->front = item;
                this->condition.notify_one();
            }
        }
        auto pop_front() -> opstate_base*
        {
            ::std::unique_lock guard(this->mutex);
            this->condition.wait(guard,
                [this]{ return this->front || this->current_state == state::finishing; });
            if (this->front == this->back)
                this->back = nullptr;
            return this->front? ::std::exchange(this->front, this->front->next): nullptr;
        }

    public:
        run_loop() noexcept = default;
        run_loop(run_loop&&) = delete;
        ~run_loop()
        {
            ::std::lock_guard guard(this->mutex);
            if (this->front != nullptr || this->current_state == state::running)
                ::std::terminate();
        }

        auto get_scheduler() -> scheduler { return {this}; }

        auto run() -> void
        {
            if (::std::lock_guard guard(this->mutex);
                this->current_state != state::finishing
                    && state::running == ::std::exchange(this->current_state, state::running))
            {
                ::std::terminate();
            }

            while (auto* op{this->pop_front()})
            {
                op->execute();
            }
        }
        auto finish() -> void
        {
            {
                ::std::lock_guard guard(this->mutex);
                this->current_state = state::finishing;
            }
            this->condition.notify_one();
        }
    };
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/get_completion_signatures.hpp     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_COMPLETION_SIGNATURES
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_GET_COMPLETION_SIGNATURES


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct get_completion_signatures_t
    {
    private:
        template <typename Sender, typename Env>
        static auto get(Sender&& sender, Env&& env) noexcept
        {
            auto new_sender{[](auto&& sender, auto&& env) -> decltype(auto) {
                return ::beman::execution26::transform_sender(
                    ::beman::execution26::detail::get_domain_late(sender, env),
                    ::std::forward<Sender>(sender),
                    ::std::forward<Env>(env)
                );
            }};

            using sender_type = ::std::remove_cvref_t<decltype(new_sender(sender, env))>;
            if constexpr (requires{ new_sender(sender, env).get_completion_signatures(env); })
                return decltype(new_sender(sender, env).get_completion_signatures(env)){};
            else if constexpr (requires{ typename sender_type::completion_signatures; })
                return typename sender_type::completion_signatures{};
            //-dk:TODO deal with awaitable
        }
    public:
        template <typename Sender, typename Env>
            requires (not ::std::same_as<void,
                decltype(get_completion_signatures_t::get(::std::declval<Sender>(), ::std::declval<Env>()))>
            )
        auto operator()(Sender&& sender, Env&& env) const noexcept
        {
            return this->get(::std::forward<Sender>(sender), ::std::forward<Env>(env));
        }
    };
    inline constexpr get_completion_signatures_t get_completion_signatures{};
}

// ----------------------------------------------------------------------------

#endif// include/beman/execution26/detail/connect.hpp                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CONNECT
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CONNECT


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    struct connect_t
    {
        template <typename Sender, typename Receiver>
        auto operator()(Sender&& sender, Receiver&& receiver) const
            noexcept(true/*-dk:TODO*/)
        {
            auto new_sender = [&sender, &receiver]() -> decltype(auto) {
                return ::beman::execution26::transform_sender(
                    decltype(
                        ::beman::execution26::detail::get_domain_late(::std::forward<Sender>(sender),
                            ::beman::execution26::get_env(receiver))
                    ){},
                    ::std::forward<Sender>(sender),
                    ::beman::execution26::get_env(receiver)
                );
            };

            if constexpr (requires{ new_sender().connect(::std::forward<Receiver>(receiver)); })
            {
                using state_type = decltype(new_sender().connect(::std::forward<Receiver>(receiver)));
                static_assert(::beman::execution26::operation_state<state_type>);
                return new_sender().connect(::std::forward<Receiver>(receiver));
            }
            else
            {
                enum bad_sender {};
                static_assert(::std::same_as<bad_sender, decltype(new_sender())>, "result from transform_sender has no suitable connect()");
            }
        }
    };
    inline constexpr connect_t connect{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/sender_in.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_IN
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_IN


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    template <typename Sender, typename Env = ::beman::execution26::empty_env>
    concept sender_in
        =  ::beman::execution26::sender<Sender>
        && ::beman::execution26::detail::queryable<Env>
        && requires(Sender&& sender, Env&& env) {
            {
                ::beman::execution26::get_completion_signatures(
                    ::std::forward<Sender>(sender),
                    ::std::forward<Env>(env)
                )
            } -> ::beman::execution26::detail::valid_completion_signatures;
        }
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/connect_result_t.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CONNECT_RESULT
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CONNECT_RESULT


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    template <typename Sender, typename Receiver>
    using connect_result_t
        = decltype(::beman::execution26::connect(::std::declval<Sender>(),
                                                 ::std::declval<Receiver>()));
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/completion_signatures_for.hpp     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_SIGNATURES_FOR
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_SIGNATURES_FOR


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    struct no_completion_signatures_defined_in_sender {};

    template <typename Sender, typename Env>
    struct completion_signatures_for_impl
    {
        using type = ::beman::execution26::detail::no_completion_signatures_defined_in_sender;
    };
    #if 0
    {
        using type = decltype(::std::invoke([]{
            if constexpr (::beman::execution26::sender_in<Sender, Env>)
                return decltype(::beman::execution26::get_completion_signatures(
                    ::std::declval<Sender>(), ::std::declval<Env>())
                ){};
            else
                return ::beman::execution26::detail::no_completion_signatures_defined_in_sender{};
        }));
    };
    #endif

    template <typename Sender, typename Env>
    using completion_signatures_for
        = ::std::conditional_t<
            ::std::same_as<
                beman::execution26::detail::no_completion_signatures_defined_in_sender,
                    typename ::beman::execution26::detail::completion_signatures_for_impl<
                        Sender, Env>::type
                >,
            typename ::beman::execution26::detail::completion_signatures_for_impl<
                ::std::remove_cvref_t<Sender>, Env>::type,
            typename ::beman::execution26::detail::completion_signatures_for_impl<
                Sender, Env>::type
        >;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/completion_signaturess_of_t.hpp   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_SIGNATURES_OF
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_COMPLETION_SIGNATURES_OF


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    template <typename Sender, typename Env = ::beman::execution26::empty_env>
        requires ::beman::execution26::sender_in<Sender>
    using completion_signatures_of_t
        = ::beman::execution26::detail::call_result_t<
            ::beman::execution26::get_completion_signatures_t, Sender, Env
        >
        ;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/connect_all.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_CONNECT_ALL
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_CONNECT_ALL


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    struct connect_all_t
    {
        static auto use(auto&&...) {}
        //-dk:TODO is the S parameter deviating from the spec?
        template <typename Sender, typename S, typename Receiver, ::std::size_t... I>
        auto operator()(::beman::execution26::detail::basic_state<Sender, Receiver>* op,
                        S&& sender,
                        ::std::index_sequence<I...>) const noexcept(true/*-dk:TODO*/)
        {
            auto data{::beman::execution26::detail::get_sender_data(::std::forward<S>(sender))};
            return ::std::apply([&op](auto&&... c){
                return [&op]<::std::size_t...J>(::std::index_sequence<J...>, auto&&... c){
                    use(op);
                    return ::beman::execution26::detail::product_type
                    {
                        ::beman::execution26::connect(
                            ::beman::execution26::detail::forward_like<Sender>(c),
                            ::beman::execution26::detail::basic_receiver<Sender, Receiver,
                                ::std::integral_constant<::size_t, J>>{op}
                        )...
                    };
                }(::std::make_index_sequence<::std::tuple_size_v<::std::decay_t<decltype(data.children)>>>{}, c...);
            }, data.children);
        }
    };
    inline constexpr connect_all_t connect_all{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/error_types_of_t.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_ERROR_TYPES_OF
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_ERROR_TYPES_OF


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    template <typename Sender,
              typename Env = ::beman::execution26::empty_env,
              template <typename...> class Variant = ::beman::execution26::detail::variant_or_empty>
        requires ::beman::execution26::sender_in<Sender, Env>
    using error_types_of_t
        = ::beman::execution26::detail::gather_signatures<
            ::beman::execution26::set_error_t,
            ::beman::execution26::completion_signatures_of_t<Sender, Env>,
            ::std::type_identity_t,
            Variant
        >;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/sends_stopped.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDS_STOPPED
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDS_STOPPED


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    template <typename Sender, typename Env = ::beman::execution26::empty_env>
        requires ::beman::execution26::sender_in<Sender, Env>
    inline constexpr bool sends_stopped{not ::std::same_as<
        ::beman::execution26::detail::type_list<>,
        ::beman::execution26::detail::gather_signatures<
            ::beman::execution26::set_stopped_t,
            ::beman::execution26::completion_signatures_of_t<Sender, Env>,
            ::beman::execution26::detail::type_list,
            ::beman::execution26::detail::type_list
        >
    >};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/value_types_of_t.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_VALUE_TYPE_OF
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_VALUE_TYPE_OF


// ----------------------------------------------------------------------------

namespace beman::execution26
{
    template <typename Sender,
             typename Env = ::beman::execution26::empty_env,
             template <typename...> class Tuple = ::beman::execution26::detail::decayed_tuple,
             template <typename...> class Variant = ::beman::execution26::detail::variant_or_empty
             >
        requires ::beman::execution26::sender_in<Sender, Env>
    using value_types_of_t
        = ::beman::execution26::detail::gather_signatures<
            ::beman::execution26::set_value_t,
            ::beman::execution26::completion_signatures_of_t<Sender, Env>,
            Tuple,
            Variant
        >;
}
// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/connect_all_result.hpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION26_DETAIL_CONNECT_ALL_RESULT
#define INCLUDED_INCLUDE_BEMAN_EXECUTION26_DETAIL_CONNECT_ALL_RESULT


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Sender, typename Receiver>
    using connect_all_result
        = ::beman::execution26::detail::call_result_t<
            decltype(::beman::execution26::detail::connect_all),
            ::beman::execution26::detail::basic_state<Sender, Receiver>*,
            Sender,
            ::beman::execution26::detail::indices_for<Sender>
        >;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/sync_wait.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SYNC_WAIT
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SYNC_WAIT


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    struct sync_wait_env
    {
        ::beman::execution26::run_loop* loop{};

        auto query(::beman::execution26::get_scheduler_t) const noexcept
        {
            return this->loop->get_scheduler();
        }
        auto query(::beman::execution26::get_delegation_scheduler_t) const noexcept
        {
            return this->loop->get_scheduler();
        }
    };

    template <::beman::execution26::sender_in Sender>
    using sync_wait_result_type
        = ::std::optional<
            ::beman::execution26::value_types_of_t<
                Sender,
                ::beman::execution26::detail::sync_wait_env,
                ::beman::execution26::detail::decayed_tuple,
                ::std::type_identity_t
            >
        >;

    template <typename Sender>
    struct sync_wait_state
    {
        ::beman::execution26::run_loop loop{};
        ::std::exception_ptr error{};

        ::beman::execution26::detail::sync_wait_result_type<Sender> result{};
    };

    template <typename Sender>
    struct sync_wait_receiver
    {
        using receiver_concept = ::beman::execution26::receiver_t;

        ::beman::execution26::detail::sync_wait_state<Sender>* state{};

        template <typename Error>
        auto set_error(Error&& error) && noexcept -> void
        {
            this->state->error
                = ::beman::execution26::detail::as_except_ptr(
                    ::std::forward<Error>(error)
                );
            this->state->loop.finish();
        }
        auto set_stopped() && noexcept -> void
        {
            this->state->loop.finish();
        }
        template <typename... Args>
        auto set_value(Args&&... args) && noexcept -> void
        {
            try
            {
                this->state->result.emplace(::std::forward<Args>(args)...);
            }
            catch(...)
            {
                this->state->error = ::std::current_exception();
            }
            this->state->loop.finish();
        }

    };

    struct sync_wait_t
    {
        template <typename Sender>
        auto apply_sender(Sender&& sender)
        {
            ::beman::execution26::detail::sync_wait_state<Sender> state;
            auto op{::beman::execution26::connect(::std::forward<Sender>(sender),
                                ::beman::execution26::detail::sync_wait_receiver<Sender>{&state})};
            ::beman::execution26::start(op);

            state.loop.run();
            if (state.error)
            {
                ::std::rethrow_exception(state.error);
            }
            return ::std::move(state.result);
        }

        template <::beman::execution26::sender_in<
            ::beman::execution26::detail::sync_wait_env> Sender>
            requires requires(Sender&& sender, sync_wait_t const& self){
                typename ::beman::execution26::detail::sync_wait_result_type<Sender>;
                {
                    ::beman::execution26::apply_sender(
                        ::beman::execution26::detail::get_domain_early(sender),
                        self,
                        ::std::forward<Sender>(sender)
                    )
                } -> ::std::same_as<
                        ::beman::execution26::detail::sync_wait_result_type<Sender>
                    >;
            }
        auto operator()(Sender&& sender) const
        {
            auto domain{::beman::execution26::detail::get_domain_early(sender)};
            return ::beman::execution26::apply_sender(domain, *this, ::std::forward<Sender>(sender));
        }
    };
}

namespace beman::execution26
{
    using sync_wait_t = ::beman::execution26::detail::sync_wait_t;
    inline constexpr ::beman::execution26::sync_wait_t sync_wait{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/basic_operation.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_OPERATION
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_OPERATION


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Sender, typename Receiver>
        requires ::beman::execution26::detail::valid_specialization<
            ::beman::execution26::detail::state_type, Sender, Receiver
        >
    struct basic_operation
        : ::beman::execution26::detail::basic_state<Sender, Receiver>
    {
        using operation_state_concept = ::beman::execution26::operation_state_t;
        using tag_t = ::beman::execution26::tag_of_t<Sender>;

        using inner_ops_t = ::beman::execution26::detail::connect_all_result<Sender, Receiver>;
        inner_ops_t inner_ops;

        template <typename S> //-dk:TODO is that deviating from the spec?
        basic_operation(S&& sender, Receiver&& receiver) noexcept(true/*-dk:TODO*/)
            : ::beman::execution26::detail::basic_state<Sender, Receiver>(
                ::std::forward<S>(sender), ::std::move(receiver)
            )
            , inner_ops(::beman::execution26::detail::connect_all(
                this,
                ::std::forward<S>(sender),
                ::beman::execution26::detail::indices_for<Sender>()
            ))
        {
        }

        auto start() & noexcept -> void
        {
            ::std::invoke([this]<::std::size_t... I>(::std::index_sequence<I...>)
                {
                    ::beman::execution26::detail::impls_for<tag_t>::start(
                        this->state,
                        this->receiver,
                        this->inner_ops.template get<I>()...
                        );
                },
                ::std::make_index_sequence<inner_ops_t::size()>{}
            );
        }
    };
    template <typename Sender, typename Receiver>
    basic_operation(Sender&&, Receiver&&) -> basic_operation<Sender&&, Receiver>;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/basic_sender.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_SENDER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_BASIC_SENDER


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Tag, typename Data, typename... Child>
    struct basic_sender
        : ::beman::execution26::detail::product_type<Tag, Data, Child...>
    {
        using sender_concept = ::beman::execution26::sender_t;
        using indices_for = ::std::index_sequence_for<Child...>;

        auto get_env() const noexcept -> decltype(auto)
        {
            auto data{::beman::execution26::detail::get_sender_data(*this)};
            return ::std::apply([&data](auto&&... c){
                return ::beman::execution26::detail::impls_for<Tag>
                    ::get_attrs(data.data, c...);
            }, data.children);
        }

        template <typename Receiver>
            requires (not ::beman::execution26::receiver<Receiver>)
        auto connect(Receiver receiver)
            = BEMAN_EXECUTION26_DELETE("the passed receiver doesn't model receiver");
#if __cpp_explicit_this_parameter < 202110L
        template <::beman::execution26::receiver Receiver>
        auto connect(Receiver receiver) &
            noexcept(true/*-dk:TODO*/)
            -> ::beman::execution26::detail::basic_operation<basic_sender&, Receiver>
        {
            return { *this, ::std::move(receiver) };
        }
        template <::beman::execution26::receiver Receiver>
        auto connect(Receiver receiver) const&
            noexcept(true/*-dk:TODO*/)
            -> ::beman::execution26::detail::basic_operation<basic_sender const&, Receiver>
        {
            return { *this, ::std::move(receiver) };
        }
        template <::beman::execution26::receiver Receiver>
        auto connect(Receiver receiver) &&
            noexcept(true/*-dk:TODO*/)
            -> ::beman::execution26::detail::basic_operation<basic_sender, Receiver>
        {
            return { ::std::move(*this), ::std::move(receiver) };
        }
#else
        template <::beman::execution26::detail::decays_to<basic_sender> Self,
                  ::beman::execution26::receiver Receiver>
        auto connect(this Self&& self, Receiver receiver)
            noexcept(true/*-dk:TODO*/)
            -> ::beman::execution26::detail::basic_operation<basic_sender, Receiver>
        {
            return { ::std::forward<Self>(self), ::std::move(receiver) };
        }
#endif
#if __cpp_explicit_this_parameter < 202110L
        template <typename Env>
        auto get_completion_signatures(Env&&) &&
            -> ::beman::execution26::detail::completion_signatures_for<basic_sender, Env>
        {
            return {};
        }
        template <typename Env>
        auto get_completion_signatures(Env&&) const&&
            -> ::beman::execution26::detail::completion_signatures_for<basic_sender, Env>
        {
            return {};
        }
        template <typename Env>
        auto get_completion_signatures(Env&&) &
            -> ::beman::execution26::detail::completion_signatures_for<basic_sender, Env>
        {
            return {};
        }
        template <typename Env>
        auto get_completion_signatures(Env&&) const& 
            -> ::beman::execution26::detail::completion_signatures_for<basic_sender, Env>
        {
            return {};
        }
#else
        template <::beman::execution26::detail::decays_to<basic_sender> Self,
                  typename Env>
        auto get_completion_signatures(this Self&&, Env&&) noexcept
            -> ::beman::execution26::detail::completion_signatures_for<Self, Env>
        {
            return {};
        }
#endif
    };
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/make_sender.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_MAKE_SENDER
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_MAKE_SENDER


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    struct make_sender_empty {};

    template <typename Tag,
              typename Data = ::beman::execution26::detail::make_sender_empty,
              typename... Child>
        requires ::std::semiregular<Tag>
        && ::beman::execution26::detail::movable_value<Data>
        && (::beman::execution26::sender<Child> && ...)
    constexpr auto make_sender(Tag tag, Data&& data, Child&&... child)
    {
        return ::beman::execution26::detail::basic_sender<
            Tag, ::std::decay_t<Data>, ::std::decay_t<Child>...
            >{tag, ::std::forward<Data>(data), ::std::forward<Child>(child)...};
    }
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/into_variant.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_INTO_VARIANT
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_INTO_VARIANT



// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    struct into_variant_t
    {
        template <::beman::execution26::sender Sender>
        auto operator()(Sender&& sender) const
        {
            auto domain{::beman::execution26::detail::get_domain_early(sender)};
            (void)domain;
            return 
                ::beman::execution26::detail::make_sender(*this, {}, ::std::forward<Sender>(sender))
                ;
            //return ::beman::execution26::transform_sender(
            //    ::std::move(domain),
            //    ::beman::execution26::detail::make_sender(*this, {}, ::std::forward<Sender>(sender))
            //);
        }
    };

    template<>
    struct impls_for<::beman::execution26::detail::into_variant_t>
        : ::beman::execution26::detail::default_impls
    {
        static constexpr auto get_state
            = []<typename Sender, typename Receiver>(Sender&&, Receiver&&) noexcept
                -> ::std::type_identity<
                        ::beman::execution26::value_types_of_t<
                            ::beman::execution26::detail::child_type<Sender>,
                            ::beman::execution26::env_of_t<Receiver>
                        >
                    >
            {
                return {};
            };
        static constexpr auto complete
            = []<typename State, typename Tag, typename... Args>
              (auto, State, auto& receiver, Tag, Args&&... args) noexcept
                -> void
            {
                if constexpr (::std::same_as<Tag, ::beman::execution26::set_value_t>)
                {
                    using variant_type = typename State::type;
                    using tuple_type = ::beman::execution26::detail::decayed_tuple<Args...>;
                    try
                    {
                        if constexpr (sizeof...(Args) == 0u)
                            ::beman::execution26::set_value(
                                ::std::move(receiver)
                            );
                        else
                            ::beman::execution26::set_value(
                                ::std::move(receiver),
                                variant_type(tuple_type{::std::forward<Args>(args)...})
                            );
                    }
                    catch(...)
                    {
                        ::beman::execution26::set_error(
                            ::std::move(receiver),
                            ::std::current_exception()
                            );
                    }
                    
                }
                else
                {
                    Tag()(::std::move(receiver), ::std::forward<Args>(args)...);
                }
            };
    };

    template <typename Sender, typename State, typename Env>
    struct completion_signatures_for_impl<
        ::beman::execution26::detail::basic_sender<
            ::beman::execution26::detail::into_variant_t,
            State,
            Sender
            >,
        Env
        >
    {
        using variant_type = ::beman::execution26::value_types_of_t<Sender, Env>;
        using value_types = ::std::conditional_t<
                ::std::same_as<variant_type, ::beman::execution26::detail::empty_variant>,
                ::beman::execution26::completion_signatures<>,
                ::beman::execution26::completion_signatures<
                    ::beman::execution26::set_value_t(variant_type)
                >
            >;
        template <typename... E>
        using make_error_types
            = ::beman::execution26::completion_signatures<::beman::execution26::set_error_t(E)...>;

        using error_types
        = ::beman::execution26::error_types_of_t<
            Sender,
            Env,
            make_error_types
            >;
        using stopped_types
            = ::std::conditional_t< 
                ::beman::execution26::sends_stopped<Sender, Env>,
                ::beman::execution26::completion_signatures<::beman::execution26::set_stopped_t()>,
                ::beman::execution26::completion_signatures<>
                >;
        using type = ::beman::execution26::detail::meta::combine<
            value_types,
            ::beman::execution26::detail::meta::combine<error_types, stopped_types>
            >;
    };
}

namespace beman::execution26
{
    using into_variant_t = ::beman::execution26::detail::into_variant_t;
    inline constexpr into_variant_t into_variant{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/just.hpp                          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_JUST
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_JUST


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Completion, typename... T>
    concept just_size
        =  (not ::std::same_as<Completion, ::beman::execution26::set_error_t>
            or 1u == sizeof...(T))
        && (not ::std::same_as<Completion, ::beman::execution26::set_stopped_t>
            or 0u == sizeof...(T))
        ;
    template <typename Completion>
    struct just_t
    {
        template <typename... T>
            requires ::beman::execution26::detail::just_size<Completion, T...>
            && (::std::movable<::std::decay_t<T>> && ...)
        auto operator()(T&&... arg) const
        {
            return ::beman::execution26::detail::make_sender(
                *this,
                ::beman::execution26::detail::product_type{::std::forward<T>(arg)...}
            );
        }
    };

    template <typename Completion, typename... T, typename Env>
    struct completion_signatures_for_impl<
        ::beman::execution26::detail::basic_sender<
            just_t<Completion>,
            ::beman::execution26::detail::product_type<T...>
            >,
        Env
        >
    {
        using type = ::beman::execution26::completion_signatures<Completion(T...)>;
    };

    template <typename Completion>
    struct impls_for<just_t<Completion>>
        : ::beman::execution26::detail::default_impls
    {
        static constexpr auto start
            =  []<typename State>(State& state, auto& receiver) noexcept -> void
            {
                [&state, &receiver]<::std::size_t... I>(::std::index_sequence<I...>)
                    {
                        Completion()(::std::move(receiver),
                                     ::std::move(state.template get<I>())...);
                    }(::std::make_index_sequence<State::size()>{});
            };
    };
}

namespace beman::execution26
{
    using just_t = ::beman::execution26::detail::just_t<::beman::execution26::set_value_t>;
    using just_error_t = ::beman::execution26::detail::just_t<::beman::execution26::set_error_t>;
    using just_stopped_t = ::beman::execution26::detail::just_t<::beman::execution26::set_stopped_t>;

    inline constexpr ::beman::execution26::just_t just{};
    inline constexpr ::beman::execution26::just_error_t just_error{};
    inline constexpr ::beman::execution26::just_stopped_t just_stopped{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/then.hpp                          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_THEN
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_THEN


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Completion>
    struct then_t
        : ::beman::execution26::sender_adaptor_closure<then_t<Completion>>
    {
        template <::beman::execution26::detail::movable_value Fun>
        auto operator()(Fun&& fun) const
        {
            return ::beman::execution26::detail::sender_adaptor{
                *this, ::std::forward<Fun>(fun)
            };
        }
        template <::beman::execution26::sender Sender,
                  ::beman::execution26::detail::movable_value Fun>
        auto operator()(Sender&& sender, Fun&& fun) const
        {
            auto domain{::beman::execution26::detail::get_domain_early(sender)};
            return ::beman::execution26::transform_sender(
                domain,
                ::beman::execution26::detail::make_sender(
                    *this,
                    ::std::forward<Fun>(fun),
                    ::std::forward<Sender>(sender)
                )
            );
        }
    };

    template <typename Completion>
    struct impls_for<then_t<Completion>>
        : ::beman::execution26::detail::default_impls
    {
        static constexpr auto complete
            = []<typename Tag, typename... Args>
                (auto, auto& fun, auto& receiver, Tag, Args&&... args)
                noexcept
                -> void
            {
                if constexpr (::std::same_as<Completion, Tag>)
                {
                    try
                    {
                        auto invoke = [&]{
                            return ::std::invoke(::std::move(fun),
                                                 ::std::forward<Args>(args)...);
                        };
                        if constexpr (::std::same_as<void, decltype(invoke())>)
                        {
                            invoke();
                            ::beman::execution26::set_value(
                                ::std::move(receiver)
                            );
                        }
                        else
                        {
                            ::beman::execution26::set_value(
                                ::std::move(receiver),
                                invoke()
                            );
                        }
                    }
                    catch (...)
                    {
                        if constexpr (not noexcept(
                            ::std::invoke(::std::move(fun),
                                                 ::std::forward<Args>(args)...)

                        ))
                        {
                            ::beman::execution26::set_error(
                                ::std::move(receiver),
                                ::std::current_exception()
                            );
                        }
                    }
                }
                else
                {
                    Tag()(::std::move(receiver), ::std::forward<Args>(args)...);
                }
            };
    };

    template <typename T>
    struct then_set_value
    {
        using type = ::beman::execution26::set_value_t(T);
    };
    template <>
    struct then_set_value<void>
    {
        using type = ::beman::execution26::set_value_t();
    };

    template <typename, typename, typename Completion>
    struct then_transform
    {
        using type = Completion;
    };

    template <typename Fun, typename Completion, typename... T>
    struct then_transform<Fun, Completion, Completion(T...)>
    {
        using type = 
            typename ::beman::execution26::detail::then_set_value<
                ::beman::execution26::detail::call_result_t<Fun, T...>
            >::type
        ;
    };

    template <typename Fun, typename Replace>
    struct then_transform_t
    {
        template <typename Completion>
        using transform = typename
        ::beman::execution26::detail::then_transform<Fun, Replace, Completion>
        ::type;
    };

    template <typename, typename, typename>
    struct then_exception_fun: ::std::false_type {};
    template <typename Comp, typename Fun, typename... A> 
    struct then_exception_fun<Comp, Fun, Comp(A...)>
        : ::std::bool_constant<not noexcept(::std::declval<Fun>()(::std::declval<A>()...))>
    {
    };

    template <typename, typename, typename> 
    struct then_exception: ::std::false_type {};
    template <typename Comp, typename Fun, typename Completion, typename... Completions> 
    struct then_exception<Comp, Fun, ::beman::execution26::completion_signatures<Completion, Completions...>>
    {
        static constexpr bool value{
            then_exception_fun<Comp, Fun, Completion>::value
            || then_exception<Comp, Fun, ::beman::execution26::completion_signatures<Completions...>>::value
            };
    };

    template <typename Completion, typename Fun, typename Sender, typename Env>
    struct completion_signatures_for_impl<
        ::beman::execution26::detail::basic_sender<
            ::beman::execution26::detail::then_t<Completion>,
            Fun,
            Sender
            >,
        Env
        >
    {
        using type = ::beman::execution26::detail::meta::unique<
            ::beman::execution26::detail::meta::combine<
                ::beman::execution26::detail::meta::transform<
                    ::beman::execution26::detail::then_transform_t<Fun, Completion>::template transform,
                    ::beman::execution26::completion_signatures_of_t<Sender, Env>
                >,
                ::std::conditional_t<
                    ::beman::execution26::detail::then_exception<
                        Completion,
                        Fun,
                        ::beman::execution26::completion_signatures_of_t<Sender, Env>
                    >::value,
                    ::beman::execution26::completion_signatures<::beman::execution26::set_error_t(::std::exception_ptr)>,
                    ::beman::execution26::completion_signatures<>
                >
            >
        >;
    };
}

namespace beman::execution26
{
    using then_t = ::beman::execution26::detail::then_t<::beman::execution26::set_value_t>;
    using upon_error_t = ::beman::execution26::detail::then_t<::beman::execution26::set_error_t>;
    using upon_stopped_t = ::beman::execution26::detail::then_t<::beman::execution26::set_stopped_t>;

    inline constexpr ::beman::execution26::then_t then{};
    inline constexpr ::beman::execution26::upon_error_t upon_error{};
    inline constexpr ::beman::execution26::upon_stopped_t upon_stopped{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/read_env.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_READ_ENV
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_READ_ENV


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    struct read_env_t
    {
        auto operator()(auto&& query) const
        {
            return ::beman::execution26::detail::make_sender(*this, query);
        }
    };

    template <>
    struct impls_for<::beman::execution26::detail::read_env_t>
        : ::beman::execution26::detail::default_impls
    {
        static constexpr auto start
            = [](auto query, auto& receiver) noexcept -> void
        {
            try
            {
                auto env{::beman::execution26::get_env(receiver)};
                ::beman::execution26::set_value(
                    ::std::move(receiver),
                    query(env)
                );
            }
            catch(...)
            {
                ::beman::execution26::set_error(
                    ::std::move(receiver),
                    ::std::current_exception()
                );
            }
            
        };
    };

    template <typename Query, typename Env>
    struct completion_signatures_for_impl<
        ::beman::execution26::detail::basic_sender<
            ::beman::execution26::detail::read_env_t,
            Query
            >,
        Env
        >
    {
        using type = ::beman::execution26::completion_signatures<
            ::beman::execution26::set_value_t(
                decltype(::std::declval<Query>()(::std::as_const(::std::declval<Env>())))
            ),
            ::beman::execution26::set_error_t(::std::exception_ptr)
            >;
    };
}

namespace beman::execution26
{
    using read_env_t = beman::execution26::detail::read_env_t;
    inline constexpr read_env_t read_env{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/detail/write_env.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_WRITE_ENV
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_WRITE_ENV


// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    struct write_env_t
    {
        template <::beman::execution26::sender Sender,
                  ::beman::execution26::detail::queryable Env>
        constexpr auto operator()(Sender&& sender, Env&& env) const
        {
            return ::beman::execution26::detail::make_sender(
                *this, ::std::forward<Env>(env), ::std::forward<Sender>(sender)
            );
        }
        static auto name() { return "write_env_t"; }
    };

    template <typename NewEnv, typename Child, typename Env>
    struct completion_signatures_for_impl<
        ::beman::execution26::detail::basic_sender<
            ::beman::execution26::detail::write_env_t, NewEnv, Child>
        , Env>
    {
        using type = decltype(::beman::execution26::get_completion_signatures(
            ::std::declval<Child>(), 
            ::beman::execution26::detail::join_env(
                ::std::declval<NewEnv>(), ::std::declval<Env>())
            ));
    };

    template <>
    struct impls_for<write_env_t>
        : ::beman::execution26::detail::default_impls
    {
        static constexpr auto get_env
            = [](auto, auto const& state, auto const& receiver) noexcept
        {
            return ::beman::execution26::detail::join_env(
                state,
                ::beman::execution26::get_env(receiver)
            );
        };
    };

    inline constexpr write_env_t write_env{};
}

// ----------------------------------------------------------------------------

#endif
// include/beman/execution26/execution.hpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_EXECUTION
#define INCLUDED_BEMAN_EXECUTION26_EXECUTION

// ----------------------------------------------------------------------------





// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/execution.hpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_NET29_DETAIL_EXECUTION
#define INCLUDED_INCLUDE_BEMAN_NET29_DETAIL_EXECUTION


// ----------------------------------------------------------------------------

namespace beman::net29::detail::ex::detail
{
    using ::beman::execution26::detail::type_list;
    using ::beman::execution26::detail::variant_or_empty;
    using ::beman::execution26::detail::meta::combine;
    using ::beman::execution26::detail::meta::filter;
    using ::beman::execution26::detail::meta::unique;
    using ::beman::execution26::detail::meta::transform;
    using ::beman::execution26::detail::sender_adaptor;
    using ::beman::execution26::detail::forward_like;
}

namespace beman::net29::detail::ex
{
    using ::beman::execution26::completion_signatures;
    using ::beman::execution26::detail::decayed_tuple;

    using ::beman::execution26::get_env;
    using ::beman::execution26::empty_env;
    using ::beman::execution26::env_of_t;
    using ::beman::execution26::value_types_of_t;
    using ::beman::execution26::error_types_of_t;

    using ::beman::execution26::get_stop_token_t;
    using ::beman::execution26::get_stop_token;
    using ::beman::execution26::get_completion_signatures_t;
    using ::beman::execution26::get_completion_signatures;
    using ::beman::execution26::get_completion_scheduler_t;
    using ::beman::execution26::get_completion_scheduler;
    using ::beman::execution26::get_delegation_scheduler_t;
    using ::beman::execution26::get_delegation_scheduler;
    using ::beman::execution26::get_scheduler_t;
    using ::beman::execution26::get_scheduler;

    using ::beman::execution26::operation_state_t;
    using ::beman::execution26::operation_state;
    using ::beman::execution26::receiver_t;
    using ::beman::execution26::receiver;
    using ::beman::execution26::sender_t;
    using ::beman::execution26::sender;
    using ::beman::execution26::scheduler_t;
    using ::beman::execution26::scheduler;

    using ::beman::execution26::sender_in;

    using ::beman::execution26::set_error_t;
    using ::beman::execution26::set_error;
    using ::beman::execution26::set_stopped_t;
    using ::beman::execution26::set_stopped;
    using ::beman::execution26::set_value_t;
    using ::beman::execution26::set_value;

    using ::beman::execution26::connect_t;
    using ::beman::execution26::connect;
    using ::beman::execution26::start_t;
    using ::beman::execution26::start;

    using ::beman::execution26::read_env;
    using ::beman::execution26::detail::write_env;
    using ::beman::execution26::just;
    using ::beman::execution26::just_error;
    using ::beman::execution26::just_stopped;
    using ::beman::execution26::then;
    using ::beman::execution26::upon_error;
    using ::beman::execution26::upon_stopped;
    using ::beman::execution26::sync_wait;
}

// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/io_context_scheduler.hpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_IO_CONTEXT_SCHEDULER
#define INCLUDED_BEMAN_NET29_DETAIL_IO_CONTEXT_SCHEDULER

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    class io_context_scheduler;
}

// ----------------------------------------------------------------------------

class beman::net29::detail::io_context_scheduler
{
private:
    ::beman::net29::detail::context_base* d_context;

public:
    using scheduler_concept = ::beman::net29::detail::ex::scheduler_t;

    struct env
    {
        ::beman::net29::detail::context_base* d_context;

        template <typename Signal>
        auto query(::beman::net29::detail::ex::get_completion_scheduler_t<Signal> const&) const noexcept
            -> io_context_scheduler
        {
            return this->d_context;
        }
    };
    struct sender
    {
        template <typename Receiver>
        struct state
            : ::beman::net29::detail::context_base::task
        {
            using operation_state_concept = ::beman::net29::detail::ex::operation_state_t;

            ::std::remove_cvref_t<Receiver>       d_receiver;
            ::beman::net29::detail::context_base* d_context;

            state(Receiver&& receiver, ::beman::net29::detail::context_base* context)
                : d_receiver(::std::forward<Receiver>(receiver))
                , d_context(context)
            {
            }

            auto start() & noexcept -> void
            {
                this->d_context->schedule(this);
            }
            auto complete() -> void override
            {
                ::beman::net29::detail::ex::set_value(::std::move(this->d_receiver));
            }
        };

        using sender_concept = ::beman::net29::detail::ex::sender_t;
        ::beman::net29::detail::context_base* d_context;

        template <typename Receiver>
        auto connect(Receiver&& receiver) -> state<Receiver>
        {
            return {::std::forward<Receiver>(receiver), this->d_context};
        }

        auto get_env() const noexcept -> env { return {this->d_context}; }
    };

    auto schedule() noexcept -> sender { return {this->d_context}; }
    auto operator== (io_context_scheduler const&) const -> bool = default;

    io_context_scheduler(::beman::net29::detail::context_base* context)
        : d_context(context)
    {
        assert(this->d_context);
    }

    auto get_context() const { return this->d_context; }

    auto cancel(beman::net29::detail::io_base* cancel_op, beman::net29::detail::io_base* op) -> void
    {
        this->d_context->cancel(cancel_op, op);
    }
    auto accept(::beman::net29::detail::context_base::accept_operation* op)
        -> ::beman::net29::detail::submit_result
    {
        return this->d_context->accept(op);
    }
    auto connect(::beman::net29::detail::context_base::connect_operation* op)
        -> ::beman::net29::detail::submit_result
    {
        return this->d_context->connect(op);
    }
    auto receive(::beman::net29::detail::context_base::receive_operation* op)
        -> ::beman::net29::detail::submit_result
    {
        return this->d_context->receive(op);
    }
    auto send(::beman::net29::detail::context_base::send_operation* op)
        -> ::beman::net29::detail::submit_result
    {
        return this->d_context->send(op);
    }
    auto resume_at(::beman::net29::detail::context_base::resume_at_operation* op)
        -> ::beman::net29::detail::submit_result
    {
        return this->d_context->resume_at(op);
    }
};

static_assert(::beman::net29::detail::ex::sender<
    beman::net29::detail::io_context_scheduler::sender
    >);
static_assert(::beman::net29::detail::ex::scheduler<
    beman::net29::detail::io_context_scheduler
    >);

// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/sender.hpp                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_SENDER
#define INCLUDED_BEMAN_NET29_DETAIL_SENDER


// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    template <::beman::net29::detail::ex::receiver>
    struct sender_state_base;
    template <::beman::net29::detail::ex::receiver>
    struct sender_upstream_receiver;
    template <typename, typename, ::beman::net29::detail::ex::receiver,
              ::beman::net29::detail::ex::sender>
    struct sender_state;
    template <typename, typename, ::beman::net29::detail::ex::sender>
    struct sender;
    template <typename>
    struct sender_cpo;
}

// ----------------------------------------------------------------------------

template <::beman::net29::detail::ex::receiver Receiver>
struct beman::net29::detail::sender_state_base
{
    Receiver           d_receiver;
    ::std::atomic<int> d_outstanding{};

    template <::beman::net29::detail::ex::receiver R>
    sender_state_base(R&& r)
        : d_receiver(::std::forward<R>(r))
    {
    }
    virtual auto start() & noexcept -> void = 0;
};

template <::beman::net29::detail::ex::receiver Receiver>
struct beman::net29::detail::sender_upstream_receiver
{
    using receiver_concept = ::beman::net29::detail::ex::receiver_t;
    ::beman::net29::detail::sender_state_base<Receiver>* d_state;

    auto set_value() && noexcept -> void
    {
        this->d_state->start();
    }
    template <typename Error>
    auto set_error(Error&& error) && noexcept -> void
    {
        ::beman::net29::detail::ex::set_error(
            ::std::move(this->d_state->d_receiver),
            ::std::forward<Error>(error)
        );
    }
    auto set_stopped() && noexcept -> void
    {
        ::beman::net29::detail::ex::set_stopped(
            ::std::move(this->d_state->d_receiver)
        );
    }
    auto get_env() const noexcept
    {
        return ::beman::net29::detail::ex::get_env(this->d_state->d_receiver);
    }
};

template <typename Desc, typename Data,
          ::beman::net29::detail::ex::receiver Receiver,
          ::beman::net29::detail::ex::sender UpstreamSender>
struct beman::net29::detail::sender_state
    : Desc::operation
    , ::beman::net29::detail::sender_state_base<Receiver>
{
    using operation_state_concept = ::beman::net29::detail::ex::operation_state_t;

    struct cancel_callback
        : ::beman::net29::detail::io_base
    {
        sender_state* d_state;
        cancel_callback(sender_state* s)
            : ::beman::net29::detail::io_base(::beman::net29::detail::socket_id(), 0)
            , d_state(s)
        {
        }
        auto operator()()
        {
            if (1 < ++this->d_state->d_outstanding)
            {
                this->d_state->d_data.get_scheduler().cancel(this, this->d_state);
            }
        }
        auto complete() -> void override final
        {
            if (0u == --this->d_state->d_outstanding)
            {
                ::beman::net29::detail::ex::set_stopped(
                    ::std::move(this->d_state->d_receiver)
                );
            }
        }
        auto error(::std::error_code) -> void override final
        {
            this->complete();
        }
        auto cancel() -> void override final
        {
            this->complete();
        }
    };
    using upstream_state_t = decltype(
        ::beman::net29::detail::ex::connect(
            ::std::declval<UpstreamSender&>(),
            ::std::declval<sender_upstream_receiver<Receiver>>()
        )
    );
    using stop_token = decltype(
        ::beman::net29::detail::ex::get_stop_token(
            ::beman::net29::detail::ex::get_env(::std::declval<Receiver const&>())
        )
    );
    using callback = typename stop_token::template callback_type<cancel_callback>;

    Data                      d_data;
    upstream_state_t          d_state;
    ::std::optional<callback> d_callback;

    template <typename D, ::beman::net29::detail::ex::receiver R>
    sender_state(D&& d, R&& r, UpstreamSender up)
        : Desc::operation(d.id(), d.events())
        , sender_state_base<Receiver>(::std::forward<R>(r))
        , d_data(::std::forward<D>(d))
        , d_state(::beman::net29::detail::ex::connect(
            up,
            sender_upstream_receiver<Receiver>{this}))
    {
    }
    auto start() & noexcept -> void override 
    {
        auto token(::beman::net29::detail::ex::get_stop_token(
                ::beman::net29::detail::ex::get_env(this->d_receiver)
            )
        );
        static_assert(not std::same_as<ex::never_stop_token, void>);
        ++this->d_outstanding;
        this->d_callback.emplace(token, cancel_callback(this));
        if (token.stop_requested())
        {
            this->d_callback.reset();
            this->cancel();
            return;
        }
        if (this->d_data.submit(this) == ::beman::net29::detail::submit_result::ready)
        {
            this->complete();
        }
    }
    auto complete() -> void override final
    {
        d_callback.reset();
        if (0 == --this->d_outstanding)
        {
            this->d_data.set_value(*this, ::std::move(this->d_receiver));
        }
    }
    auto error(::std::error_code err) -> void override final
    {
        d_callback.reset();
        if (0 == --this->d_outstanding)
        {
            ::beman::net29::detail::ex::set_error(
                ::std::move(this->d_receiver),
                std::move(err)
            );
        }
    }
    auto cancel() -> void override final
    {
        if (0 == --this->d_outstanding)
        {
            ::beman::net29::detail::ex::set_stopped(::std::move(this->d_receiver));
        }
    }
};

template <typename Desc, typename Data, ::beman::net29::detail::ex::sender Upstream>
struct beman::net29::detail::sender
{
    using sender_concept = ::beman::net29::detail::ex::sender_t;
    using completion_signatures
        = ::beman::net29::detail::ex::completion_signatures<
            typename Data::completion_signature,
            ::beman::net29::detail::ex::set_error_t(::std::error_code),
            ::beman::net29::detail::ex::set_stopped_t()
            >;

    Data     d_data;
    Upstream d_upstream;

    template <::beman::net29::detail::ex::receiver Receiver>
    auto connect(Receiver&& receiver) const&
    {
        return ::beman::net29::detail::sender_state<Desc, Data,
            ::std::remove_cvref_t<Receiver>, Upstream>(
            this->d_data,
            ::std::forward<Receiver>(receiver),
            this->d_upstream
            );
    }
    template <::beman::net29::detail::ex::receiver Receiver>
    auto connect(Receiver&& receiver) &&
    {
        return ::beman::net29::detail::sender_state<Desc, Data,
            ::std::remove_cvref_t<Receiver>, Upstream>(
            this->d_data,
            ::std::forward<Receiver>(receiver),
            this->d_upstream
            );
    }
};

template <typename Desc>
struct beman::net29::detail::sender_cpo
{
    template <typename Arg0, typename... Args>
        requires (!::beman::net29::detail::ex::sender<::std::remove_cvref_t<Arg0>>)
            && ::std::invocable<sender_cpo const,
                           decltype(::beman::net29::detail::ex::just()),
                           Arg0, Args...>
    auto operator()(Arg0&& arg0, Args&&... args) const
    {
        return (*this)(::beman::net29::detail::ex::just(),
                       ::std::forward<Arg0>(arg0),
                       ::std::forward<Args>(args)...);
    }
    template <::beman::net29::detail::ex::sender Upstream, typename... Args>
    auto operator()(Upstream&& u, Args&&... args) const
    {
        using data = Desc::template data<::std::decay_t<Args>...>;
        return ::beman::net29::detail::sender<Desc, data, ::std::remove_cvref_t<Upstream>>{
            data{::std::forward<Args>(args)...},
            ::std::forward<Upstream>(u)
        };
    }
};

// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/basic_socket.hpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_BASIC_SOCKET
#define INCLUDED_BEMAN_NET29_DETAIL_BASIC_SOCKET

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

template <typename Protocol>
class beman::net29::basic_socket
    : public ::beman::net29::socket_base
{
public:
    using scheduler_type     = ::beman::net29::detail::io_context_scheduler;
    using protocol_type      = Protocol;

private:
    ::beman::net29::detail::context_base* d_context;
    protocol_type                     d_protocol{::beman::net29::ip::tcp::v6()}; 
    ::beman::net29::detail::socket_id     d_id{::beman::net29::detail::socket_id::invalid};

public:
    basic_socket()
        : d_context(nullptr)
    {
    }
    basic_socket(::beman::net29::detail::context_base* context, ::beman::net29::detail::socket_id id)
        : d_context(context)
        , d_id(id)
    {
    }
    basic_socket(basic_socket&& other)
        : d_context(other.d_context)
        , d_protocol(other.d_protocol)
        , d_id(::std::exchange(other.d_id, ::beman::net29::detail::socket_id::invalid))
    {
    }
    ~basic_socket()
    {
        if (this->d_id != ::beman::net29::detail::socket_id::invalid)
        {
            ::std::error_code error{};
            this->d_context->release(this->d_id, error);
        }
    }
    auto get_scheduler() noexcept -> scheduler_type
    {
        return scheduler_type{this->d_context};
    }
    auto id() const -> ::beman::net29::detail::socket_id { return this->d_id; }
};


// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/io_context.hpp                          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_IO_CONTEXT
#define INCLUDED_BEMAN_NET29_DETAIL_IO_CONTEXT

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

namespace beman::net29
{
    class io_context;
}

// ----------------------------------------------------------------------------

class beman::net29::io_context
{
private:
    ::std::unique_ptr<::beman::net29::detail::context_base> d_owned{new ::beman::net29::detail::poll_context()};
    ::beman::net29::detail::context_base&                   d_context{*this->d_owned};

public:
    using scheduler_type = ::beman::net29::detail::io_context_scheduler;
    class executor_type {};

    io_context() { std::signal(SIGPIPE, SIG_IGN); }
    io_context(::beman::net29::detail::context_base& context): d_owned(), d_context(context) {}
    io_context(io_context&&) = delete;

    auto make_socket(int d, int t, int p, ::std::error_code& error) -> ::beman::net29::detail::socket_id
    {
        return this->d_context.make_socket(d, t, p, error);
    }
    auto release(::beman::net29::detail::socket_id id, ::std::error_code& error) -> void
    {
        return this->d_context.release(id, error);
    }
    auto native_handle(::beman::net29::detail::socket_id id) -> ::beman::net29::detail::native_handle_type
    {
        return this->d_context.native_handle(id);
    }
    auto set_option(::beman::net29::detail::socket_id id,
                     int level,
                     int name,
                     void const* data,
                     ::socklen_t size,
                     ::std::error_code& error) -> void
    {
        this->d_context.set_option(id, level, name, data, size, error);
    }
    auto bind(::beman::net29::detail::socket_id id, ::beman::net29::ip::basic_endpoint<::beman::net29::ip::tcp> const& endpoint, ::std::error_code& error)
    {
        this->d_context.bind(id, ::beman::net29::detail::endpoint(endpoint), error);
    }
    auto listen(::beman::net29::detail::socket_id id, int no, ::std::error_code& error)
    {
        this->d_context.listen(id, no, error);
    }
    auto get_scheduler() -> scheduler_type { return scheduler_type(&this->d_context); }

    ::std::size_t run_one() { return this->d_context.run_one(); }
    ::std::size_t run()
    {
        ::std::size_t count{};
        while (::std::size_t c = this->run_one())
        {
            count += c;
        }
        return count;
    }
};

// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/operations.hpp                          -*-C++-*-
// ----------------------------------------------------------------------------
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// ----------------------------------------------------------------------------

#ifndef INCLUDED_INCLUDE_BEMAN_NET29_DETAIL_OPERATIONS
#define INCLUDED_INCLUDE_BEMAN_NET29_DETAIL_OPERATIONS


// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    struct accept_desc;
    struct connect_desc;
    struct send_desc;
    struct send_to_desc;
    struct receive_desc;
    struct receive_from_desc;
}

namespace beman::net29
{
    using async_accept_t
        = ::beman::net29::detail::sender_cpo<::beman::net29::detail::accept_desc>;
    inline constexpr async_accept_t async_accept{};

    using async_connect_t
        = ::beman::net29::detail::sender_cpo<::beman::net29::detail::connect_desc>;
    inline constexpr async_connect_t async_connect{};

    using async_send_t
        = ::beman::net29::detail::sender_cpo<::beman::net29::detail::send_desc>;
    inline constexpr async_send_t async_send{};

    using async_send_to_t
        = ::beman::net29::detail::sender_cpo<::beman::net29::detail::send_to_desc>;
    inline constexpr async_send_to_t async_send_to{};

    using async_receive_t
        = ::beman::net29::detail::sender_cpo<::beman::net29::detail::receive_desc>;
    inline constexpr async_receive_t async_receive{};
    using async_receive_from_t
        = ::beman::net29::detail::sender_cpo<::beman::net29::detail::receive_from_desc>;
    inline constexpr async_receive_from_t async_receive_from{};
}

// ----------------------------------------------------------------------------

struct beman::net29::detail::accept_desc
{
    using operation = ::beman::net29::detail::context_base::accept_operation;
    template <typename Acceptor>
    struct data
    {
        using acceptor_t = ::std::remove_cvref_t<Acceptor>;
        using socket_t = acceptor_t::socket_type;
        using completion_signature
            = ::beman::net29::detail::ex::set_value_t(
                socket_t,
                typename socket_t::endpoint_type
            );

        acceptor_t& d_acceptor;
        data(acceptor_t& a): d_acceptor(a) {}

        auto id() const { return this->d_acceptor.id(); }
        auto events() const { return POLLIN; }
        auto get_scheduler() { return this->d_acceptor.get_scheduler(); }
        auto set_value(operation& o, auto&& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver),
                                 socket_t(this->d_acceptor.get_scheduler().get_context(),
                                           ::std::move(*::std::get<2>(o))),
                                 typename socket_t::endpoint_type(::std::get<0>(o)));
        }
        auto submit(auto* base) -> ::beman::net29::detail::submit_result
        {
            ::std::get<1>(*base) = sizeof(::std::get<0>(*base));
            return this->get_scheduler().accept(base);
        }
    };
};

struct beman::net29::detail::connect_desc
{
    using operation = ::beman::net29::detail::context_base::connect_operation;
    template <typename Socket>
    struct data
    {
        using completion_signature = ::beman::net29::detail::ex::set_value_t();

        Socket& d_socket;

        auto id() const { return this->d_socket.id(); }
        auto events() const { return POLLIN | POLLOUT; }
        auto get_scheduler() { return this->d_socket.get_scheduler(); }
        auto set_value(operation&, auto&& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver));
        }
        auto submit(auto* base) -> ::beman::net29::detail::submit_result
        {
            ::std::get<0>(*base) = this->d_socket.get_endpoint();
            return this->d_socket.get_scheduler().connect(base);
        }
    };
};

struct beman::net29::detail::send_desc
{
    using operation = ::beman::net29::detail::context_base::send_operation;
    template <typename Stream_t, typename Buffers>
    struct data
    {
        using completion_signature = ::beman::net29::detail::ex::set_value_t(::std::size_t);

        Stream_t& d_stream;
        Buffers   d_buffers;

        auto id() const { return this->d_stream.id(); }
        auto events() const { return POLLOUT; }
        auto get_scheduler() { return this->d_stream.get_scheduler(); }
        auto set_value(operation& o, auto&& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver),
                                 ::std::move(::std::get<2>(o)));
        }
        auto submit(auto* base) -> ::beman::net29::detail::submit_result
        {
            ::std::get<0>(*base).msg_iov    = this->d_buffers.data();
            ::std::get<0>(*base).msg_iovlen = this->d_buffers.size();
            return this->d_stream.get_scheduler().send(base);
        }
    };
};

struct beman::net29::detail::send_to_desc
{
    using operation = ::beman::net29::detail::context_base::send_operation;
    template <typename Stream_t, typename Buffers, typename Endpoint>
    struct Data
    {
        using completion_signature = ::beman::net29::detail::ex::set_value_t(::std::size_t);

        Stream_t& d_stream;
        Buffers   d_buffers;
        Endpoint  d_endpoint;

        auto id() const { return this->d_stream.id(); }
        auto events() const { return POLLOUT; }
        auto get_scheduler() { return this->d_stream.get_scheduler(); }
        auto set_value(operation& o, auto&& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver), ::std::get<2>(o));
        }
        auto submit(auto* base) -> ::beman::net29::detail::submit_result
        {
            ::std::get<0>(*base).msg_iov     = this->d_buffers.data();
            ::std::get<0>(*base).msg_iovlen  = this->d_buffers.size();
            ::std::get<0>(*base).msg_name    = this->d_endpoint.data();
            ::std::get<0>(*base).msg_namelen = this->d_endpoint.size();
            return this->d_stream.get_scheduler().send(base);
        }
    };
};

struct beman::net29::detail::receive_desc
{
    using operation = ::beman::net29::detail::context_base::receive_operation;
    template <typename Stream_t, typename Buffers>
    struct data
    {
        using completion_signature = ::beman::net29::detail::ex::set_value_t(::std::size_t);

        Stream_t& d_stream;
        Buffers   d_buffers;

        auto id() const { return this->d_stream.id(); }
        auto events() const { return POLLIN; }
        auto get_scheduler() { return this->d_stream.get_scheduler(); }
        auto set_value(operation& o, auto&& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver), ::std::get<2>(o));
        }
        auto submit(auto* base) -> ::beman::net29::detail::submit_result
        {
            ::std::get<0>(*base).msg_iov    = this->d_buffers.data();
            ::std::get<0>(*base).msg_iovlen = this->d_buffers.size();
            return this->d_stream.get_scheduler().receive(base);
        }
    };
};

struct beman::net29::detail::receive_from_desc
{
    using operation = ::beman::net29::detail::context_base::receive_operation;
    template <typename Stream_t, typename Buffers, typename Endpoint>
    struct data
    {
        using completion_signature = ::beman::net29::detail::ex::set_value_t(::std::size_t);

        Stream_t& d_stream;
        Buffers   d_buffers;
        Endpoint  d_endpoint;

        auto id() const { return this->d_stream.id(); }
        auto events() const { return POLLIN; }
        auto get_scheduler() { return this->d_stream.get_scheduler(); }
        auto set_value(operation& o, auto&& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver), ::std::get<2>(o));
        }
        auto submit(auto* base) -> ::beman::net29::detail::submit_result
        {
            ::std::get<0>(*base).msg_iov     = this->d_buffers.data();
            ::std::get<0>(*base).msg_iovlen  = this->d_buffers.size();
            ::std::get<0>(*base).msg_name    = this->d_buffers.data();
            ::std::get<0>(*base).msg_namelen = this->d_buffers.size();
            return this->d_stream.get_scheduler().receive(base);
        }
    };
};


// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/timer.hpp                               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_TIMER
#define INCLUDED_BEMAN_NET29_DETAIL_TIMER

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    struct resume_after_desc;
    struct resume_at_desc;
}

namespace beman::net29
{
    using async_resume_after_t = ::beman::net29::detail::sender_cpo<::beman::net29::detail::resume_after_desc>;
    using async_resume_at_t    = ::beman::net29::detail::sender_cpo<::beman::net29::detail::resume_at_desc>;

    inline constexpr async_resume_after_t resume_after{};
    inline constexpr async_resume_at_t    resume_at{};
}

// ----------------------------------------------------------------------------

struct beman::net29::detail::resume_after_desc
{
    using operation = ::beman::net29::detail::context_base::resume_after_operation;
    template <typename Scheduler, typename>
    struct data
    {
        using completion_signature = ::beman::net29::detail::ex::set_value_t();

        ::std::remove_cvref_t<Scheduler>        d_scheduler;
        ::std::chrono::microseconds             d_duration;

        auto id() const -> ::beman::net29::detail::socket_id { return {}; }
        auto events() const { return decltype(POLLIN)(); }
        auto get_scheduler() { return this->d_scheduler; }
        auto set_value(operation&, auto&& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver));
        }
        auto submit(auto* base) -> ::beman::net29::detail::submit_result
        {
            ::std::get<0>(*base) = ::std::chrono::system_clock::now() + this->d_duration;
            return this->d_scheduler.resume_at(base);
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

        ::std::remove_cvref_t<Scheduler>        d_scheduler;
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
// include/beman/net29/detail/basic_socket_acceptor.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_BASIC_SOCKET_ACCEPTOR
#define INCLUDED_BEMAN_NET29_DETAIL_BASIC_SOCKET_ACCEPTOR


// ----------------------------------------------------------------------------

namespace beman::net29
{
    template <typename>
    class basic_socket_acceptor;
}

// ----------------------------------------------------------------------------

template <typename AcceptableProtocol>
class beman::net29::basic_socket_acceptor
    : public ::beman::net29::socket_base
{
public:
    using scheduler_type     = ::beman::net29::io_context::scheduler_type;
    using executor_type      = ::beman::net29::io_context::executor_type;
    using native_handle_type = ::beman::net29::detail::native_handle_type;
    using protocol_type      = AcceptableProtocol;
    using endpoint_type      = typename protocol_type::endpoint;
    using socket_type        = typename protocol_type::socket;

private:
    ::beman::net29::io_context&       d_context;
    protocol_type                     d_protocol; 
    ::beman::net29::detail::socket_id d_id{};

private:
    template <typename Fun_t>
    static void dispatch(Fun_t&& fun)
    {
        ::std::error_code error{};
        fun(error);
        if (error)
        {
            throw ::std::system_error(error);
        }
    }

public:
    //explicit basic_socket_acceptor(::beman::net29::io_context&);
    basic_socket_acceptor(::beman::net29::io_context&, protocol_type const& protocol);
    basic_socket_acceptor(::beman::net29::io_context& context, endpoint_type const& endpoint, bool reuse = true)
        : ::beman::net29::socket_base()
        , d_context(context)
        , d_protocol(endpoint.protocol())
        , d_id(::beman::net29::detail::socket_id::invalid)
    {
        this->open(endpoint.protocol());
        if (reuse)
        {
            this->set_option(::beman::net29::socket_base::reuse_address(true));
        }
        this->bind(endpoint);
        this->listen();
    }
    basic_socket_acceptor(::beman::net29::io_context&, protocol_type const&, native_handle_type const&);
    basic_socket_acceptor(basic_socket_acceptor const&) = delete;
    basic_socket_acceptor(basic_socket_acceptor&& other)
        : ::beman::net29::socket_base()
        , d_protocol(other.d_protocol)
        , d_id(::std::exchange(other.d_id, ::beman::net29::detail::socket_id::invalid))
    {
    }
    template<typename OtherProtocol>
    basic_socket_acceptor(::beman::net29::basic_socket_acceptor<OtherProtocol>&&);
    ~basic_socket_acceptor()
    {
        //-dk:TODO assert that there is no outstanding work?
        ::std::error_code error{};
        this->close(error);
    }
    basic_socket_acceptor& operator=(basic_socket_acceptor const&) = delete;
    basic_socket_acceptor& operator=(basic_socket_acceptor&&);
    template<typename OtherProtocol>
    basic_socket_acceptor& operator=(::beman::net29::basic_socket_acceptor<OtherProtocol>&&);

    auto get_context() -> ::beman::net29::io_context& { return this->d_context; }
    auto get_scheduler() noexcept -> scheduler_type
    {
        return this->d_context.get_scheduler();
    }
    executor_type      get_executor() noexcept;
    auto native_handle() -> native_handle_type { return this->d_context.native_handle(this->d_id); }
    auto _native_handle() const -> native_handle_type { return this->d_context.native_handle(this->d_id); }
    auto id() const -> ::beman::net29::detail::socket_id { return this->d_id; }
    auto open(protocol_type const& p = protocol_type()) -> void
    {
        dispatch([this, &p](::std::error_code& error){ this->open(p, error); });
    }
    auto open(protocol_type const& p, ::std::error_code& error) -> void
    {
        if (this->is_open())
        {
            error = ::std::error_code(int(socket_errc::already_open), ::beman::net29::socket_category());
        }
        this->d_id = this->d_context.make_socket(p.family(), p.type(), p.protocol(), error);
    }
    void assign(protocol_type const&, native_handle_type const&);
    void assign(protocol_type const&, native_handle_type const&, ::std::error_code&);
    native_handle_type release();
    native_handle_type release(::std::error_code&);
    auto is_open() const noexcept -> bool { return this->d_id != ::beman::net29::detail::socket_id::invalid; }
    auto close() -> void
    {
        dispatch([this](auto& error){ return this->close(error); });
    }
    auto close(::std::error_code& error) -> void
    {
        //-dk:TODO cancel outstanding work
        if (this->is_open())
        {
            this->d_context.release(this->id(), error);
            this->d_id = ::beman::net29::detail::socket_id::invalid;
        }
    }
    void cancel();
    void cancel(::std::error_code&);
    template<typename SettableSocketOption>
    auto set_option(SettableSocketOption const& option) -> void
    {
        dispatch([this, option](::std::error_code& error){ this->set_option(option, error); });
    }
    template<typename SettableSocketOption>
    auto set_option(SettableSocketOption const& option, ::std::error_code& error) -> void
    {
        this->d_context.set_option(
            this->id(),
            option.level(this->d_protocol),
            option.name(this->d_protocol),
            option.data(this->d_protocol),
            option.size(this->d_protocol),
            error);
    }

    template<typename GettableSocketOption>
    void get_option(GettableSocketOption&) const;
    template<typename gettableSocketOption>
    void get_option(gettableSocketOption&, ::std::error_code&) const;
    template<typename IoControlCommand>
    void io_control(IoControlCommand&);
    template<typename IoControlCommand>
    void io_control(IoControlCommand&, ::std::error_code&);
    void non_blocking(bool);
    void non_blocking(bool, ::std::error_code&);
    bool non_blocking() const;
    void native_non_blocking(bool);
    void native_non_blocking(bool, ::std::error_code&);
    bool native_non_blocking() const;
    auto bind(endpoint_type const& endpoint) -> void
    {
        this->dispatch([this, endpoint](::std::error_code& error){ this->bind(endpoint, error); });
    }
    auto bind(endpoint_type const& endpoint, ::std::error_code& error) -> void
    {
        this->d_context.bind(this->d_id, endpoint, error);
    }
    auto listen(int no = ::beman::net29::socket_base::max_listen_connections) -> void
    {
        dispatch([this, no](auto& error){ this->listen(no, error); });
    }
    auto listen(int no, ::std::error_code& error) -> void
    {
        this->d_context.listen(this->d_id, no, error);
    }
    endpoint_type local_endpoint() const;
    endpoint_type local_endpoint(::std::error_code&) const;
    void enable_connection_aborted(bool);
    bool enable_connection_aborted() const;
    socket_type accept();
    socket_type accept(::std::error_code&);
    socket_type accept(io_context&);
    socket_type accept(io_context&, ::std::error_code&);
    socket_type accept(endpoint_type&);
    socket_type accept(endpoint_type&, ::std::error_code&);
    socket_type accept(::beman::net29::io_context&, endpoint_type&);
    socket_type accept(::beman::net29::io_context&, endpoint_type&, ::std::error_code&);
    void wait(::beman::net29::socket_base::wait_type);
    void wait(::beman::net29::socket_base::wait_type, ::std::error_code&);
};

// ----------------------------------------------------------------------------

#endif
// include/beman/net29/detail/basic_stream_socket.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_BASIC_STREAM_SOCKET
#define INCLUDED_BEMAN_NET29_DETAIL_BASIC_STREAM_SOCKET

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

template <typename Protocol>
class beman::net29::basic_stream_socket
    : public basic_socket<Protocol>
{
public:
    using native_handle_type = ::beman::net29::detail::native_handle_type;
    using protocol_type = Protocol;
    using endpoint_type = typename protocol_type::endpoint;

private:
    endpoint_type d_endpoint;

public:
    basic_stream_socket(basic_stream_socket&&) = default;
    basic_stream_socket& operator= (basic_stream_socket&&) = default;
    basic_stream_socket(::beman::net29::detail::context_base* context, ::beman::net29::detail::socket_id id)
        : basic_socket<Protocol>(context, id)
    {
    }
    basic_stream_socket(::beman::net29::io_context& context, endpoint_type const& endpoint)
        : beman::net29::basic_socket<Protocol>(context.get_scheduler().get_context(),
            ::std::invoke([p = endpoint.protocol(), &context]{
                ::std::error_code error{};
                auto rc(context.make_socket(p.family(), p.type(), p.protocol(), error));
                if (error)
                {
                    throw ::std::system_error(error);
                }
                return rc;
            }))
        , d_endpoint(endpoint) 
    {
    }

    auto get_endpoint() const -> endpoint_type { return this->d_endpoint; }
};


// ----------------------------------------------------------------------------

#endif
// include/beman/net29/net.hpp                                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_NET
#define INCLUDED_BEMAN_NET29_NET

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

#endif



#if defined(__clang__)
#    pragma clang diagnostic ignored "-Wreturn-type"
#endif

// ----------------------------------------------------------------------------

namespace demo
{
    namespace ex = ::beman::net29::detail::ex;

    template <typename T>
    struct task_state_base
    {
        ::std::optional<T> task_result;
        virtual auto complete_value() -> void = 0;
        virtual auto complete_error(::std::exception_ptr) -> void = 0;
        virtual auto complete_stopped() -> void = 0;

        template <typename Receiver>
        auto complete_set_value(Receiver& receiver)
        {
            ::beman::net29::detail::ex::set_value(
                ::std::move(receiver), ::std::move(*this->task_result)
            );
        }
    };
    template <>
    struct task_state_base<void>
    {
        virtual auto complete_value() -> void = 0;
        virtual auto complete_error(::std::exception_ptr) -> void = 0;
        virtual auto complete_stopped() -> void = 0;

        template <typename Receiver>
        auto complete_set_value(Receiver& receiver)
        {
            ::beman::net29::detail::ex::set_value(::std::move(receiver));
        }
    };

    struct task_none {};
    template <typename... T> struct task_type_or_none;
    template <typename T> struct task_type_or_none<T> { using type = T; };
    template <> struct task_type_or_none<> { using type = task_none; };
    template <typename... T> using task_type_or_none_t = typename task_type_or_none<T...>::type;

    template <typename... T>
    struct task_single_or_tuple { using type = ::std::tuple<::std::decay_t<T>...>; };
    template <typename T>
    struct task_single_or_tuple<T> { using type = ::std::decay_t<T>; };
    template <typename... T>
    using task_single_or_tuple_t = typename task_single_or_tuple<T...>::type;

    template <typename R>
    struct task_promise_result
    {
        task_state_base<R>* state{};
        template <typename T>
        auto return_value(T&& r) -> void
        {
            this->state->task_result.emplace(std::forward<T>(r));
        }
    };
    template <>
    struct task_promise_result<void>
    {
        task_state_base<void>* state{};
        auto return_void() -> void
        {
        }
    };

    template <typename T>
    struct task_completion { using type = ::beman::net29::detail::ex::set_value_t(T); };
    template <>
    struct task_completion<void> { using type = ::beman::net29::detail::ex::set_value_t(); };

    template <typename Result = void>
    struct task
    {
        enum class stop_state { running, stopping, stopped };
        template <typename Promise, ex::sender Sender>
        struct sender_awaiter
        {
            struct env
            {
                sender_awaiter* awaiter{};
                auto query(ex::get_stop_token_t) const noexcept -> ex::inplace_stop_token;
            };
            struct receiver
            {
                using receiver_concept = ex::receiver_t;

                sender_awaiter* awaiter{};

                template <typename... Args>
                auto set_value(Args&&... args) noexcept -> void
                {
                    this->awaiter->result.emplace(::std::forward<Args>(args)...);
                    this->awaiter->handle.resume();
                }
                template <typename Error>
                auto set_error(Error&& error) noexcept -> void
                {
                    if constexpr (::std::same_as<::std::decay_t<Error>, ::std::exception_ptr>)
                        this->awaiter->error = error;
                    else
                        this->awaiter->error = ::std::make_exception_ptr(::std::forward<Error>(error));
                    this->awaiter->handle.resume();
                }
                auto set_stopped() noexcept -> void
                {
                    this->awaiter->stop();
                }
                auto get_env() const noexcept -> env { return {this->awaiter}; }
            };
            using value_type
                = ex::value_types_of_t<
                    Sender, Promise, task_single_or_tuple_t, task_type_or_none_t>;
            using state_type = decltype(ex::connect(::std::declval<Sender>(), std::declval<receiver>()));

            ::std::coroutine_handle<Promise> handle;
            ::std::exception_ptr             error;
            ::std::optional<value_type>      result;
            state_type                       state;

            sender_awaiter(Sender sender)
                : state(ex::connect(::std::move(sender), receiver{this}))
            {
            }

            auto stop() -> void;
            auto get_token() const noexcept -> ex::inplace_stop_token;
            constexpr auto await_ready() const noexcept -> bool { return false; }
            auto await_suspend(::std::coroutine_handle<Promise> handle) -> void
            {
                this->handle = handle;
                ex::start(this->state);
            }
            auto await_resume()
            {
                if (this->error)
                    std::rethrow_exception(this->error);
                return ::std::move(*this->result);
            }
        };

        struct promise_type;
        struct final_awaiter
        {
            promise_type* promise;
            constexpr auto await_ready() const noexcept -> bool { return false; }
            auto await_suspend(::std::coroutine_handle<>) noexcept -> void
            {
                this->promise->state->complete_value();
            }
            constexpr auto await_resume() const noexcept -> void {}
        };

        struct promise_type
            : task_promise_result<Result>
        {
            task::stop_state        stop_state{task::stop_state::running};
            ex::inplace_stop_source stop_source{};

            auto initial_suspend() -> ::std::suspend_always { return {}; }
            auto final_suspend() noexcept -> final_awaiter { return {this}; }
            auto get_return_object() -> task
            {
                return {unique_handle(this)};
            }
            auto unhandled_exception() -> void
            {
                this->state->complete_error(::std::current_exception());
            }
            template <ex::sender Sender>
            auto await_transform(Sender&& sender)
            {
                return sender_awaiter<promise_type, ::std::remove_cvref_t<Sender>>
                     (::std::forward<Sender>(sender));
            }
        };

        using deleter = decltype([](promise_type* p) {
            std::coroutine_handle<promise_type>::from_promise(*p).destroy();
        });
        using unique_handle= std::unique_ptr<promise_type, deleter>;

        template <typename Receiver>
        struct state
            : task_state_base<::std::decay_t<Result>>
        {
            using operation_state_concept = ex::operation_state_t;
            struct callback_t
            {
                state* object;
                auto operator()() const
                {
                    auto state{this->object};
                    state->callback.reset();
                    state->handle->stop_state = task::stop_state::stopping;
                    state->handle->stop_source.request_stop();
                    if (state->handle->stop_state == task::stop_state::stopped)
                        this->object->handle->state->complete_stopped();
                }
            };
            using stop_token = decltype(ex::get_stop_token(ex::get_env(::std::declval<Receiver>())));
            using stop_callback = ex::stop_callback_for_t<stop_token, callback_t>;

            unique_handle                  handle;
            ::std::decay_t<Receiver>       receiver;
            ::std::optional<stop_callback> callback;

            template <typename R>
            state(unique_handle handle, R&& receiver)
                : handle(::std::move(handle))
                , receiver(::std::forward<R>(receiver))
            {
            }

            auto start() & noexcept -> void
            {
                this->handle->state = this;
                this->callback.emplace(ex::get_stop_token(ex::get_env(this->receiver)), callback_t{this});
                std::coroutine_handle<promise_type>::from_promise(*this->handle).resume();
            }

            auto complete_value() -> void override
            {
                this->complete_set_value(this->receiver);
            }
            auto complete_error(::std::exception_ptr error) -> void override
            {
                ::beman::net29::detail::ex::set_error(
                    ::std::move(this->receiver),
                    ::std::move(error)
                );
            }
            auto complete_stopped() -> void override
            {
                ::beman::net29::detail::ex::set_stopped(::std::move(this->receiver));
            }
        };

        unique_handle handle;

        using sender_concept = ::beman::net29::detail::ex::sender_t;
        using completion_signatures = ::beman::net29::detail::ex::completion_signatures<
            ::beman::net29::detail::ex::set_error_t(::std::exception_ptr),
            ::beman::net29::detail::ex::set_stopped_t(),
            typename task_completion<::std::decay_t<Result>>::type
        >;

        template <typename Receiver>
        auto connect(Receiver&& receiver)
        {
            return state<Receiver>(
                ::std::move(this->handle),
                ::std::forward<Receiver>(receiver)
            );
        }
    };
}

// ----------------------------------------------------------------------------

template <typename Result>
    template <typename Promise, demo::ex::sender Sender>
auto
demo::task<Result>::sender_awaiter<Promise, Sender>::env::query(demo::ex::get_stop_token_t) const noexcept
    -> demo::ex::inplace_stop_token
{
    return this->awaiter->handle.promise().stop_source.get_token();
}

template <typename Result>
    template <typename Promise, demo::ex::sender Sender>
auto
demo::task<Result>::sender_awaiter<Promise, Sender>::stop() ->void
{
    if (::std::exchange(this->handle.promise().stop_state, task::stop_state::stopped)
        == task::stop_state::running)
    {
        this->handle.promise().state->complete_stopped();
    }
}

// ----------------------------------------------------------------------------

#endif
