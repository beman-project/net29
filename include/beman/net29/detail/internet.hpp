// include/beman/net29/detail/internet.hpp                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_INTERNET
#define INCLUDED_BEMAN_NET29_DETAIL_INTERNET

#include <beman/net29/detail/netfwd.hpp>
#include <beman/net29/detail/endpoint.hpp>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <array>
#include <compare>
#include <cstdint>
#include <cstring>
#include <ostream>
#include <stdexcept>
#include <string>

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
    int _D_family;

    constexpr tcp(int _F): _D_family(_F) {}

public:
    using endpoint = basic_endpoint<tcp>;
    using socket   = basic_stream_socket<tcp>;
    using acceptor = basic_socket_acceptor<tcp>;

    tcp() = delete;

    static constexpr auto v4() -> tcp { return tcp(PF_INET); }
    static constexpr auto v6() -> tcp { return tcp(PF_INET6); }

    constexpr auto family() const -> int { return this->_D_family; }
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
    uint_type _D_address;

public:
    constexpr address_v4() noexcept: _D_address() {}
    constexpr address_v4(address_v4 const&) noexcept = default;
    constexpr address_v4(bytes_type const&);
    explicit constexpr address_v4(uint_type _A)
        : _D_address(_A)
    {
        if (!(_A <= 0xFF'FF'FF'FF))
        {
            throw ::std::out_of_range("IPv4 address is out of range");
        }
    }

    auto operator=(const address_v4& a) noexcept -> address_v4& = default;

    constexpr auto is_unspecified() const noexcept -> bool { return this->to_uint() == 0u; }
    constexpr auto is_loopback() const noexcept -> bool { return (this->to_uint() & 0xFF'00'00'00) == 0x7F'00'00'00; }
    constexpr auto is_multicast() const noexcept -> bool{ return (this->to_uint() & 0xF0'00'00'00) == 0xE0'00'00'00; }
    constexpr auto to_bytes() const noexcept -> bytes_type;
    constexpr auto to_uint() const noexcept -> uint_type  { return this->_D_address; }
    template<typename _Allocator = ::std::allocator<char>>
    auto to_string(const _Allocator& = _Allocator()) const
        -> ::std::basic_string<char, ::std::char_traits<char>, _Allocator>;

    static constexpr auto any() noexcept -> address_v4 { return address_v4(); }
    static constexpr auto loopback() noexcept -> address_v4 { return address_v4(0x7F'00'00'01u); }
    static constexpr auto broadcast() noexcept -> address_v4 { return address_v4(0xFF'FF'FF'FFu); }

    friend ::std::ostream& operator<< (::std::ostream& _Out, address_v4 const& _A)
    {
        return _Out << ((_A._D_address >> 24) & 0xFFu) << '.'
                    << ((_A._D_address >> 16) & 0xFFu) << '.'
                    << ((_A._D_address >>  8) & 0xFFu) << '.'
                    << ((_A._D_address >>  0) & 0xFFu)
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
        template <typename... _Tt>
        explicit constexpr bytes_type(_Tt... _T)
            : std::array<unsigned char, 16>{{ static_cast<unsigned char>(_T)... }}
        {
        }
    };

private:
    bytes_type _D_bytes;

public:
    static constexpr auto any() noexcept -> address_v6;
    static constexpr auto loopback() noexcept -> address_v6;

    constexpr address_v6() noexcept;
    constexpr address_v6(address_v6 const&) noexcept = default;
    constexpr address_v6(unsigned char const (&_Addr)[16]) noexcept
    {
        ::std::memcpy(_D_bytes.data(), _Addr, 16);
    }

    auto operator= (address_v6 const&) noexcept -> address_v6& = default;
    constexpr auto operator== (address_v6 const&) const -> bool = default;
    constexpr auto operator<=> (address_v6 const&) const -> ::std::strong_ordering;

    auto _Get_address(::sockaddr_in6& _Addr, ::beman::net29::ip::port_type _Port) const
        -> ::socklen_t
    {
        _Addr.sin6_family = AF_INET6;
        _Addr.sin6_port = htons(_Port);
        _Addr.sin6_flowinfo = 0;
        ::std::memcpy(_Addr.sin6_addr.s6_addr, this->_D_bytes.data(), 16);
        _Addr.sin6_scope_id = 0;
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

    friend ::std::ostream& operator<< (::std::ostream& _Out, address_v6 const&)
    {
        //-dk:TODO
        return _Out << "<TODO>::1";
    }
};

inline constexpr beman::net29::ip::address_v6::address_v6() noexcept
    : _D_bytes()
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
    union _Address_t
    {
        ::sockaddr_storage _Storage;
        ::sockaddr_in      _Inet;
        ::sockaddr_in6     _Inet6;
    };
    
    _Address_t _D_address;

public:
    constexpr address() noexcept
        : _D_address()
    {
        this->_D_address._Storage.ss_family = PF_INET;
    }
    constexpr address(address const&) noexcept = default;
    /*-dk:TODO constexpr*/ address(::beman::net29::ip::address_v4 const& _Address) noexcept
    {
        this->_D_address._Inet.sin_family = AF_INET;
        this->_D_address._Inet.sin_addr.s_addr = htonl(_Address.to_uint());
        this->_D_address._Inet.sin_port = 0xFF'FF;
    }
    /*-dk:TODO constexpr*/ address(::beman::net29::ip::address_v6 const& _Address) noexcept
    {
        _Address._Get_address(this->_D_address._Inet6, 0xFF'FF);
    }

    auto operator=(address const&) noexcept -> address& = default;
    auto operator=(::beman::net29::ip::address_v4 const&) noexcept -> address&;
    auto operator=(::beman::net29::ip::address_v6 const&) noexcept -> address&;

    auto _Data() const -> ::sockaddr_storage const& { return this->_D_address._Storage; }
    constexpr auto is_v4() const noexcept -> bool { return this->_D_address._Storage.ss_family == PF_INET; }
    constexpr auto is_v6() const noexcept -> bool { return this->_D_address._Storage.ss_family == PF_INET6; }
    /*constexpr -dk:TODO*/ auto to_v4() const -> ::beman::net29::ip::address_v4
    {
        return ::beman::net29::ip::address_v4(ntohl(reinterpret_cast<::sockaddr_in const&>(this->_D_address._Storage).sin_addr.s_addr));
    }
    constexpr auto to_v6() const -> ::beman::net29::ip::address_v6
    {
        return ::beman::net29::ip::address_v6(this->_D_address._Inet6.sin6_addr.s6_addr);
    }
    constexpr auto is_unspecified() const noexcept -> bool;
    constexpr auto is_loopback() const noexcept -> bool;
    constexpr auto is_multicast() const noexcept -> bool;
    template<class _Allocator = ::std::allocator<char>>
    auto to_string(_Allocator const& = _Allocator()) const
        -> ::std::basic_string<char, ::std::char_traits<char>, _Allocator>;
    friend ::std::ostream& operator<< (::std::ostream& _Out, address const& _A)
    {
        if (_A.is_v4())
            return _Out << _A.to_v4();
        else
            return _Out << _A.to_v6();
    }
};

// ----------------------------------------------------------------------------

template <typename _Protocol>
class beman::net29::ip::basic_endpoint
    : public ::beman::net29::detail::endpoint
{
public:
    using protocol_type = _Protocol;

    constexpr basic_endpoint() noexcept
        : basic_endpoint(::beman::net29::ip::address(), ::beman::net29::ip::port_type())
    {
    }
    constexpr basic_endpoint(::beman::net29::detail::endpoint const& _Ep) noexcept
        : ::beman::net29::detail::endpoint(_Ep)
    {
    }
    constexpr basic_endpoint(const protocol_type&, ::beman::net29::ip::port_type) noexcept;
    constexpr basic_endpoint(const ip::address& _Address, ::beman::net29::ip::port_type _Port) noexcept
        : ::beman::net29::detail::endpoint(&_Address._Data(), _Address.is_v4()? sizeof(::sockaddr_in): sizeof(::sockaddr_in6))
    {
        (_Address.is_v4()
         ? reinterpret_cast<::sockaddr_in&>(this->_Storage()).sin_port
         : reinterpret_cast<::sockaddr_in6&>(this->_Storage()).sin6_port) = htons(_Port);
    }

    constexpr auto protocol() const noexcept -> protocol_type
    {
        return this->_Storage().ss_family == PF_INET? ::beman::net29::ip::tcp::v4(): ::beman::net29::ip::tcp::v6();
    }
    /*-dk:TODO constexpr*/ auto address() const noexcept -> ::beman::net29::ip::address
    {
        switch (this->_Storage().ss_family)
        {
        default: return {};
        case PF_INET: return ::beman::net29::ip::address_v4(ntohl(reinterpret_cast<::sockaddr_in const&>(this->_Storage()).sin_addr.s_addr));
        //-dk:TODO case PF_INET6: return ::beman::net29::ip::address_v6(reinterpret_cast<::sockaddr_in6 const&>(this->_Storage()).sin6_addr.s_addr);
        }
    }
    auto address(::beman::net29::ip::address const&) noexcept -> void;
    constexpr auto port() const noexcept -> ::beman::net29::ip::port_type
    {
        switch (this->_Storage().ss_family)
        {
            default: return {};
            case PF_INET: return ntohs(reinterpret_cast<::sockaddr_in const&>(this->_Storage()).sin_port);
            case PF_INET6: return ntohs(reinterpret_cast<::sockaddr_in6 const&>(this->_Storage()).sin6_port);
        }
    }
    auto port(::beman::net29::ip::port_type) noexcept -> void;

    auto _Size() const -> ::socklen_t
    {
        return this->_Storage().ss_family == PF_INET? sizeof(::sockaddr_in): sizeof(::sockaddr_in6);
    }

    friend ::std::ostream& operator<< (std::ostream& _Out, basic_endpoint const& _Ep)
    {
        return _Out << _Ep.address() << ":" << _Ep.port();
    }
};

// ----------------------------------------------------------------------------

#endif
