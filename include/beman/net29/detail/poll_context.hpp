// include/beman/net29/detail/poll_context.hpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_NET29_DETAIL_POLL_CONTEXT
#define INCLUDED_BEMAN_NET29_DETAIL_POLL_CONTEXT

// ----------------------------------------------------------------------------

#include <beman/net29/detail/netfwd.hpp>
#include <beman/net29/detail/container.hpp>
#include <beman/net29/detail/context_base.hpp>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

// ----------------------------------------------------------------------------

namespace beman::net29::detail
{
    struct poll_record;
    struct poll_context;
}

// ----------------------------------------------------------------------------

struct beman::net29::detail::poll_record final
{
    poll_record(::beman::net29::detail::native_handle_type _H): _Handle(_H) {}
    ::beman::net29::detail::native_handle_type                   _Handle;
    bool                                                   _Blocking{true};
};

// ----------------------------------------------------------------------------

struct beman::net29::detail::poll_context final
    : ::beman::net29::detail::context_base
{
    ::beman::net29::detail::container<::beman::net29::detail::poll_record> _D_sockets;
    ::std::vector<::pollfd>     _D_poll;
    ::std::vector<::beman::net29::detail::io_base*> _D_outstanding;

    auto _Make_socket(int _Fd) -> ::beman::net29::detail::socket_id override final
    {
        return this->_D_sockets._Insert(_Fd);
    }
    auto _Make_socket(int _D, int _T, int _P, ::std::error_code& _Error)
        -> ::beman::net29::detail::socket_id override final
    {
        int _Fd(::socket(_D, _T, _P));
        if (_Fd < 0)
        {
            _Error = ::std::error_code(errno, ::std::system_category());
            return ::beman::net29::detail::socket_id::invalid;
        }
        return this->_Make_socket(_Fd);
    }
    auto _Release(::beman::net29::detail::socket_id _Id, ::std::error_code& _Error) -> void override final
    {
        ::beman::net29::detail::native_handle_type _Handle(this->_D_sockets[_Id]._Handle);
        this->_D_sockets._Erase(_Id);
        if (::close(_Handle) < 0)
        {
            _Error = ::std::error_code(errno, ::std::system_category());
        }
    }
    auto _Native_handle(::beman::net29::detail::socket_id _Id) -> ::beman::net29::detail::native_handle_type override final
    {
        return this->_D_sockets[_Id]._Handle;
    }
    auto _Set_option(::beman::net29::detail::socket_id _Id,
                     int _Level,
                     int _Name,
                     void const* _Data,
                     ::socklen_t _Size,
                     ::std::error_code& _Error) -> void override final
    {
        if (::setsockopt(this->_Native_handle(_Id), _Level, _Name, _Data, _Size) < 0)
        {
            _Error = ::std::error_code(errno, ::std::system_category());
        }
    }
    auto _Bind(::beman::net29::detail::socket_id _Id,
               ::beman::net29::detail::endpoint const& endpoint,
               ::std::error_code& _Error) -> void override final
    {
        if (::bind(this->_Native_handle(_Id), endpoint.data(), endpoint.size()) < 0)
        {
            _Error = ::std::error_code(errno, ::std::system_category());
        }
    }
    auto _Listen(::beman::net29::detail::socket_id _Id, int _No, ::std::error_code& _Error) -> void override final
    {
        if (::listen(this->_Native_handle(_Id), _No) < 0)
        {
            _Error = ::std::error_code(errno, ::std::system_category());
        }
    }

    auto run_one() -> ::std::size_t override final
    {
        if (this->_D_poll.empty())
        {
            return ::std::size_t{};
        }
        while (true)
        {
            int _Rc(::poll(this->_D_poll.data(), this->_D_poll.size(), -1));
            if (_Rc < 0)
            {
                switch (errno)
                {
                default:
                    return ::std::size_t();
                case EINTR:
                case EAGAIN:
                    break;
                }
            }
            else
            {
                for (::std::size_t _I(this->_D_poll.size()); 0 < _I--; )
                {
                    if (this->_D_poll[_I].revents & (this->_D_poll[_I].events | POLLERR))
                    {
                        ::beman::net29::detail::io_base* _Completion = this->_D_outstanding[_I];
                        if (_I + 1u != this->_D_poll.size())
                        {
                            this->_D_poll[_I] = this->_D_poll.back();
                            this->_D_outstanding[_I] = this->_D_outstanding.back();
                        }
                        this->_D_poll.pop_back();
                        this->_D_outstanding.pop_back();
                        _Completion->_Work(*this, _Completion);
                        return ::std::size_t(1);
                    }
                }
            }
        }
        return ::std::size_t{};
    }
    auto _Wakeup() -> void
    {
        //-dk:TODO wake-up polling thread
    }

    auto _Add_Outstanding(::beman::net29::detail::io_base* _Completion) -> bool
    {
        auto _Id{_Completion->_Id};
        if (this->_D_sockets[_Id]._Blocking || !_Completion->_Work(*this, _Completion))
        {
            this->_D_poll.emplace_back(::pollfd{this->_Native_handle(_Id), short(_Completion->_Event), short()});
            this->_D_outstanding.emplace_back(_Completion);
            this->_Wakeup();
            return false;
        }
        return true;
    }

    auto _Cancel(::beman::net29::detail::io_base*, ::beman::net29::detail::io_base*) -> void override final
    {
        //-dk:TODO
    }
    auto _Accept(::beman::net29::detail::context_base::_Accept_operation* _Completion)
        -> bool override final
    {
        _Completion->_Work =
            [](::beman::net29::detail::context_base& _Ctxt, ::beman::net29::detail::io_base* _Comp)
            {
                auto _Id{_Comp->_Id};
                auto& _Completion(*static_cast<_Accept_operation*>(_Comp));

                while (true)
                {
                    int _Rc = ::accept(_Ctxt._Native_handle(_Id), ::std::get<0>(_Completion).data(), &::std::get<1>(_Completion));
                    if (0 <= _Rc)
                    {
                        ::std::get<2>(_Completion) =  _Ctxt._Make_socket(_Rc);
                        _Completion.complete();
                        return true;
                    }
                    else
                    {
                        switch (errno)
                        {
                        default:
                            _Completion.error(::std::error_code(errno, ::std::system_category()));
                            return true;
                        case EINTR:
                            break;
                        case EWOULDBLOCK:
                            return false;
                        }
                    }
                }
            };
        return this->_Add_Outstanding(_Completion);
    }
    auto _Connect(::beman::net29::detail::context_base::_Connect_operation*) -> bool override { return {}; /*-dk:TODO*/ } 
    auto _Receive(::beman::net29::detail::context_base::_Receive_operation*) -> bool override { return {}; /*-dk:TODO*/ }
    auto _Send(::beman::net29::detail::context_base::_Send_operation*) -> bool override { return {}; /*-dk:TODO*/ }
    auto _Resume_after(::beman::net29::detail::context_base::_Resume_after_operation*) -> bool override
    {
        //-dk:TODO
        return {};
    }
    auto _Resume_at(::beman::net29::detail::context_base::_Resume_at_operation*) -> bool override
    {
        //-dk:TODO
        return {};
    }
};

// ----------------------------------------------------------------------------

#endif
