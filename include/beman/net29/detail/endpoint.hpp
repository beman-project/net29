// include/beman/net29/detail/endpoint.hpp                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_ENDPOINT
#define INCLUDED_BEMAN_NET29_DETAIL_ENDPOINT

#include <algorithm>
#include <cstring>
#include <sys/socket.h>

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
