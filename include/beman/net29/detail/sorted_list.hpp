// include/beman/net29/detail/sorted_list.hpp                         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_SORTED_LIST
#define INCLUDED_BEMAN_NET29_DETAIL_SORTED_LIST

#include <functional>

// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    template <typename, typename = ::std::less<>> struct sorted_list;
}

// ----------------------------------------------------------------------------

template <typename T, typename Compare>
struct beman::net29::detail::sorted_list
{
    Compare comp{};
    T*      head{};

    auto empty() const -> bool { return this->head == nullptr; }
    auto front() const -> T* { return this->head; }
    auto pop_front() -> T*;
    auto insert(T* node) -> T*;
    auto erase(T*) -> T*;
};

template <typename T, typename Compare>
auto beman::net29::detail::sorted_list<T, Compare>::pop_front() -> T*
{
    T* rc{this->head};
    this->head = rc->next;
    return rc;
}

template <typename T, typename Compare>
auto beman::net29::detail::sorted_list<T, Compare>::insert(T* node) -> T*
{
    T** it{&this->head};
    while (*it != nullptr && comp((*it)->value, node->value))
        it = &(*it)->next;
    node->next = *it;
    *it = node;
    return node;
}

template <typename T, typename Compare>
auto beman::net29::detail::sorted_list<T, Compare>::erase(T* node) -> T*
{
    T** it{&this->head};
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
