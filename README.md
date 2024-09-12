# beman.net29: Senders For Network Operations

`beman.net29` provides senders for asynchronous network operations.
It is based on [P2762](http://wg21.link/p2762). Both the proposal
and the implementation are in an early state although basic
functionality is provided.

`beman.net29` builds on top of [`beman.execution26`](). The
implementation should allow multiple ways how to support asynchronous
operations although currently the only implemented approach is based
on `poll(2)`. Implementations using `kqueue(2)`, `epoll(2)`,
`io_uring`, and IOCP hopefully follow soon. It should also be
possible use various libraries for asynchronous operations like
`libevent`, `libuv`, etc.

## Building

Currently, the interfaces are entirely implemented in headers, i.e.,
there isn't a library needed. However, using the library `libbeman_net29`
should make the relevant paths to headers available.  The following
instructions build the library and the examples (required libraries are
obtained and build using these instructions):

    cmake -B build
    cmake --build build

The code is working on MacOS building with the system's
[`clang`](https://clang.llvm.org/) and recent [`gcc`](http://gcc.gnu.org/).
It should hopefully also work on Linux but it isn't tested, yet.
Windows isn't supported, yet, primarily because the relevant class
for the networking operations are different.