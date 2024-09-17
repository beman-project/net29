// examples/demo_error.hpp                                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_EXAMPLES_DEMO_ERROR
#define INCLUDED_EXAMPLES_DEMO_ERROR

#include <string>
#include <system_error>
#include <exception>
#include <coroutine>
#include <memory>
#include <tuple>
#include <optional>
#include <type_traits>
#include <atomic>
#include <utility>
#include <variant>
#include <version>
#include <iostream>
#include <concepts>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <cstddef>
#include <cassert>
#include <limits>
#include <cstdint>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <array>
#include <compare>
#include <cstring>
#include <ostream>
#include <stdexcept>
#include <chrono>
#include <sys/time.h>
#include <cerrno>
#include <csignal>
#include <algorithm>



// ----------------------------------------------------------------------------

namespace demo
{
    inline constexpr int timeout{1};

    inline auto category() -> std::error_category const&
    {
        struct category
            : std::error_category
        {
            auto name() const noexcept -> char const* override {
                return "demo-category";
            }
            auto message(int c) const noexcept -> std::string override {
                return c == 1? "timeout": "unknown";
            }
        };
        static category rc{};
        return rc;
    }
}

// ----------------------------------------------------------------------------

#endif
