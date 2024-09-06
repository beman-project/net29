// examples/demo_scope.hpp                                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_EXAMPLES_DEMO_SCOPE
#define INCLUDED_EXAMPLES_DEMO_SCOPE

#include <beman/net29/net.hpp>

// ----------------------------------------------------------------------------

namespace demo
{
    namespace ex = ::beman::net29::detail::ex;

    class scope
    {
    public:
        auto spawn(ex::sender auto&&) {}
    };
}

// ----------------------------------------------------------------------------

#endif
