# aychTTP

C++ HTTP server primarily for HTMX.

Read the [2 part blog series](https://www.amn.sh/blog/2025-12-14-http-server-in-cpp) for an in-depth tutorial on HTTP in C++.

There's a suprisingly low number of HTMX servers written in low-level C++. Or HTTP servers in general. Most that I've seen use something like Drogon or Boost.Beast (both are phenomenal libraries).
At this point in the project, the only major features left are architectural improvements and testing.

This repo serves as an example of how low-level networking concepts are still highly relevant for web development.
Sure, for ergonomics you probably don't want to use C++, GoLang, Python, and friends would be nicer.
But, you can certainly learn much more about how the web actually works by using less conveniant APIs.
And, who knows, maybe the performance could be better than the most recent JS framework!

---

Uses Boost.Asio for networking. Asio gives a nice level of control.
It's much more portable than OS-specific sockets libraries. It also has a much nicer API.
Boost.Beast or other higher level HTTP libraries are better choices for production usage,
but for learning's sake it's nice to have a lower-level library like asio.

---

HTMX is a phenomenal library in the modern sea of JS frameworks.
It's a great use-case for a hand-written HTTP server.
HTMX is also a client (alongside a browser, of course), so it's nice not to have to write an HTTP client yourself as well.
CORS responses are interesting, as usually you don't have to deal with them if you're writing your own HTTP client.

---

To run the server, just build and run the project. If you edited the test_client without rebuilding the server, you'll need to copy the new files to the build directory.
Then, open http://localhost:3000/ in your browser.
No hot reloading is supported.

---

To add more routes, check the `response_handler.h` and `response_handler.cpp` files.

The templating engine uses `Inja`, which is highly similar to `Jinja2` if you're familiar with it.

## Building

All you need is CMake, vcpkg (in your PATH or environment), a C++23 compiler, and a generator (Visual Studio, Ninja, Makefiles, etc).

Use one of the presets! It enables the vcpkg integration as long as `VCPKG_ROOT` is set.

i.e.
- `cmake --preset debug` && `cmake --build --preset debug-build`
- `cmake --preset release` && `cmake --build --preset release-build`
- `cmake --preset release -G "Visual Studio 18 2026"` && `cmake --build --preset release-build`
- `cmake --preset release -DCMAKE_CXX_COMPILER=clang++` && `cmake --build --preset release-build`

## Technical Notes

The server uses C++ Coroutines with Boost.Asio's coroutine support for concurrency.

I had at once intended to use C++ 20 modules, but Boost Asio's support was way too shoddy.

If you want, check out the commit `3a4c4296` for a version that uses C++ modules but doesn't compile.
