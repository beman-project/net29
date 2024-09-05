// include/beman/net29/detail/container.hpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_CONTAINER
#define INCLUDED_BEMAN_NET29_DETAIL_CONTAINER

#include <beman/net29/detail/netfwd.hpp>
#include <cstddef>
#include <variant>
#include <vector>

// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    template <typename> class container;
}

// ----------------------------------------------------------------------------

template <typename _Record>
class beman::net29::detail::container
{
private:
    ::std::vector<::std::variant<::std::size_t, _Record>> _Records;
    ::std::size_t                                         _Free{};

public:
    auto _Insert(_Record _R) -> ::beman::net29::detail::socket_id;
    auto _Erase(::beman::net29::detail::socket_id _Id) -> void;
    auto operator[](::beman::net29::detail::socket_id _Id) -> _Record&;
};

// ----------------------------------------------------------------------------

template <typename _Record>
inline auto beman::net29::detail::container<_Record>::_Insert(_Record _R) -> ::beman::net29::detail::socket_id
{
    if (this->_Free == this->_Records.size())
    {
        this->_Records.emplace_back(::std::move(_R));
        return ::beman::net29::detail::socket_id(this->_Free++);
    }
    else
    {
        ::std::size_t _Rc(std::exchange(this->_Free, ::std::get<0>(this->_Records[this->_Free])));
        this->_Records[_Rc] = ::std::move(_R);
        return ::beman::net29::detail::socket_id(_Rc);
    }
}

template <typename _Record>
inline auto beman::net29::detail::container<_Record>::_Erase(::beman::net29::detail::socket_id _Id) -> void
{
    this->_Records[::std::size_t(_Id)] = std::exchange(this->_Free, ::std::size_t(_Id));
}

template <typename _Record>
inline auto beman::net29::detail::container<_Record>::operator[](::beman::net29::detail::socket_id _Id) -> _Record&
{
    return ::std::get<1>(this->_Records[::std::size_t(_Id)]);
}

// ----------------------------------------------------------------------------

#endif