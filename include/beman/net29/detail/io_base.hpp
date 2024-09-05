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
    using extra_t = ::std::unique_ptr<void, auto(*)(void*)->void>;

    io_base*                         next{nullptr}; // used for an intrusive list
    ::beman::net29::detail::context_base* context{nullptr};
    ::beman::net29::detail::socket_id     id;            // the entity affected
    int                               event;         // mask for expected events
    auto                            (*work)(::beman::net29::detail::context_base&, io_base*) -> bool = nullptr;
    extra_t                          extra{nullptr, +[](void*){}};

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
