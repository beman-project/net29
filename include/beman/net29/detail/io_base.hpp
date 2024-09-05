// include/beman/net29/detail/io_base.hpp                             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_IO_BASE
#define INCLUDED_BEMAN_NET29_DETAIL_IO_BASE

#include <beman/net29/detail/netfwd.hpp>
#include <memory>
#include <system_error>

// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    struct io_base;
    template <typename> struct io_operation;
}

// ----------------------------------------------------------------------------
// The struct io_base is used as base class of operation states. Objects of
// this type are also used to kick off the actual work once a readiness
// indication was received.

struct beman::net29::detail::io_base
{
    using _Extra_t = ::std::unique_ptr<void, auto(*)(void*)->void>;

    io_base*                         _Next{nullptr}; // used for an intrusive list
    ::beman::net29::detail::context_base* _Context{nullptr};
    ::beman::net29::detail::socket_id     _Id;            // the entity affected
    int                               _Event;         // mask for expected events
    auto                            (*_Work)(::beman::net29::detail::context_base&, io_base*) -> bool = nullptr;
    _Extra_t                          _Extra{nullptr, +[](void*){}};

    io_base(::beman::net29::detail::socket_id _Id, int _Event): _Id(_Id), _Event(_Event) {}

    virtual auto complete() -> void = 0;
    virtual auto error(::std::error_code) -> void = 0;
    virtual auto cancel() -> void = 0;
};


// ----------------------------------------------------------------------------
// The struct io_operation is an io_base storing operation specific data.

template <typename _Data>
struct beman::net29::detail::io_operation
    : io_base
    , _Data
{
    template <typename _D = _Data>
    io_operation(::beman::net29::detail::socket_id _Id, int _Event, _D&& _A = _Data())
        : io_base(_Id, _Event)
        , _Data(::std::forward<_D>(_A))
    {
    }
};


// ----------------------------------------------------------------------------

#endif
