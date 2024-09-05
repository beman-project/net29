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
    ::beman::net29::detail::context_base* _D_context;

public:
    io_context_scheduler(::beman::net29::detail::context_base* _Context)
        : _D_context(_Context)
    {
        assert(this->_D_context);
    }

    auto _Get_context() const { return this->_D_context; }

    auto _Cancel(beman::net29::detail::io_base* _Cancel_op, beman::net29::detail::io_base* _Op) -> void
    {
        this->_D_context->_Cancel(_Cancel_op, _Op);
    }
    auto _Accept(::beman::net29::detail::context_base::_Accept_operation* _Op) -> bool
    {
        return this->_D_context->_Accept(_Op);
    }
    auto _Connect(::beman::net29::detail::context_base::_Connect_operation* _Op) -> bool
    {
        return this->_D_context->_Connect(_Op);
    }
    auto _Receive(::beman::net29::detail::context_base::_Receive_operation* _Op) -> bool
    {
        return this->_D_context->_Receive(_Op);
    }
    auto _Send(::beman::net29::detail::context_base::_Send_operation* _Op) -> bool
    {
        return this->_D_context->_Send(_Op);
    }
    auto _Resume_after(::beman::net29::detail::context_base::_Resume_after_operation* _Op) -> bool
    {
        return this->_D_context->_Resume_after(_Op);
    }
    auto _Resume_at(::beman::net29::detail::context_base::_Resume_at_operation* _Op) -> bool
    {
        return this->_D_context->_Resume_at(_Op);
    }
};

// ----------------------------------------------------------------------------

#endif
