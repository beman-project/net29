// include/beman/net29/detail/buffer.hpp                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_BUFFER
#define INCLUDED_BEMAN_NET29_DETAIL_BUFFER

#include <sys/socket.h>
#include <string>
#include <system_error>
#include <cassert>
#include <cstddef>

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
