# aychTTP

C++ HTTP server primarily for HTMX. 
Uses Boost.Asio for networking. Asio gives a nice level of control.
It's much more portable than OS-specific sockets libraries. It also has a much nicer API.
Boost.Beast or other higher level HTTP libraries are better choices for production usage, 
but for learning's sake it's nice to have a lower-level library like asio.

HTMX is a phenomenal library in the modern sea of JS frameworks.
It is also a use-case for a manually written HTTP server.
HTMX is also a client (alongside a browser, of course), so it's nice not to have to write an HTTP client myself as well.
CORS responses are interesting, as usually you don't have to deal with them if you're writing your own HTTP client.

To run the server, just build and run the project. Make sure you copy the test_client directory into the same directory as the executable (bin/debug/ or bin/release/).
Then, open http://localhost:3000/ in your browser.

To add more routes, check the `response_handler.h` and `response_handler.cpp` files.

## Building

All you need is CMake, vcpkg (in your PATH or environment), a C++23 compiler, and a generator (Visual Studio, Ninja, Makefiles, etc).

Use one of the presets! It enables the vcpkg integration as long as `VCPKG_ROOT` is set.

i.e.
- `cmake --preset debug` && `cmake --build --preset debug-build`
- `cmake --preset release` && `cmake --build --preset release-build`
- `cmake --preset release -G "Visual Studio 19 2026"` && `cmake --build --preset release-build`
- `cmake --preset release -DCMAKE_CXX_COMPILER=clang++` && `cmake --build --preset release-build`
