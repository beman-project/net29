// include/beman/net29/detail/io_context_scheduler.hpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_IO_CONTEXT_SCHEDULER
#define INCLUDED_BEMAN_NET29_DETAIL_IO_CONTEXT_SCHEDULER

// ----------------------------------------------------------------------------

#include <beman/net29/detail/context_base.hpp>
#include <cassert>

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
    auto accept(::beman::net29::detail::context_base::accept_operation* op) -> bool
    {
        return this->d_context->accept(op);
    }
    auto connect(::beman::net29::detail::context_base::connect_operation* op) -> bool
    {
        return this->d_context->connect(op);
    }
    auto receive(::beman::net29::detail::context_base::receive_operation* op) -> bool
    {
        return this->d_context->receive(op);
    }
    auto send(::beman::net29::detail::context_base::send_operation* op) -> bool
    {
        return this->d_context->send(op);
    }
    auto resume_after(::beman::net29::detail::context_base::resume_after_operation* op) -> bool
    {
        return this->d_context->resume_after(op);
    }
    auto resume_at(::beman::net29::detail::context_base::resume_at_operation* op) -> bool
    {
        return this->d_context->resume_at(op);
    }
};

// ----------------------------------------------------------------------------

#endif
