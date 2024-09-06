// include/beman/net29/detail/socket_category.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_SOCKET_CATEGORY
#define INCLUDED_BEMAN_NET29_DETAIL_SOCKET_CATEGORY

#include <system_error>

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
