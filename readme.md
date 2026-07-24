### `constexpr std::shared_ptr` and friends

R6 [https://isocpp.org/files/papers/P3037R6.pdf](https://isocpp.org/files/papers/P3037R6.pdf)

R5 [https://isocpp.org/files/papers/P3037R5.pdf](https://isocpp.org/files/papers/P3037R5.pdf)

R4 [https://isocpp.org/files/papers/P3037R4.pdf](https://isocpp.org/files/papers/P3037R4.pdf)

R3 [https://isocpp.org/files/papers/P3037R3.pdf](https://isocpp.org/files/papers/P3037R3.pdf)

R2 [https://isocpp.org/files/papers/P3037R2.pdf](https://isocpp.org/files/papers/P3037R2.pdf)

R1 [https://isocpp.org/files/papers/P3037R1.pdf](https://isocpp.org/files/papers/P3037R1.pdf)

R0 [https://isocpp.org/files/papers/P3037R0.pdf](https://isocpp.org/files/papers/P3037R0.pdf)

The default `constexpr` branch contains a `constexpr` implementation of
`std::shared_ptr`, now adopted for C++26 (P3037). The code is based on the
`std::shared_ptr` implementation in GCC's libstdc++; and many of the
`constexpr` unit tests included here are derived from the libstdc++ testsuite.
The implementation is presented as a modification of a pre-installed set
of libstdc++ header files; and the `include` directory provided here can
potentially be used, along with a recent version of GCC (installed here at
`/opt/gcc-latest/`), or Clang, via:

```
$ $CXX -Wl,-rpath,"/opt/gcc-latest/lib64:$LD_LIBRARY_PATH" -L /opt/gcc-latest/lib64 -std=c++26 -Winvalid-constexpr -fsanitize=address -I $PWD/include/c++/17.0.0 -I $PWD/include/c++/17.0.0/x86_64-pc-linux-gnu main.cpp
```

Most of the changes required for this implementation are within five files:

```
include/c++/17.0.0/bits/atomic_base.h
include/c++/17.0.0/bits/out_ptr.h
include/c++/17.0.0/bits/shared_ptr.h
include/c++/17.0.0/bits/shared_ptr_atomic.h
include/c++/17.0.0/bits/shared_ptr_base.h
```

With P3037 adopted, the standard feature-test macro is `__cpp_lib_constexpr_memory`,
bumped to `202506L`; `include/c++/17.0.0/bits/version.h` has been edited by hand
to add this C++26 tier (upstream GCC does not yet implement the feature, so its
generated `version.h` lacks it). The macro is defined when the -std=c++26 flag
has been set, and the header guards here test the internal
`__glibcxx_constexpr_memory` variant, following libstdc++ convention.

One enabler of `constexpr` `shared_ptr` is the implementation of adopted
proposal P2738 in GCC and Clang; which allows `constexpr` casting from `void*`;
visible in the `get_deleter` implementation. `constexpr` incompatible atomic
operations have been avoided, under the assumption that constant expression
evaluation will be single-threaded (see `_M_get_use_count` in
`shared_ptr_base.h` for example). Then, the libstdc++ implementation of the
`std::make_shared*` and `std::allocate_shared*` families were making use of a
single (untyped) allocation to store both the control block, and the managed
element(s). This was relying on casts which are not permitted by C++26's P2738
support. (See `_Guarded_ptr` and elsewhere.) Consequently, a new function was
added (`cest_allocate_shared`), which allocates the managed elements
separately, before relying on the existing ternary `std::shared_ptr`
constructor. This is defined in `shared_ptr.h`, and used there by all
`std::make_shared*` and `std::allocate_shared*` function templates.
Clang-specific modifications are also included within the pointer handling
section of the `compare_three_way` implementation in the `compare header`. It
is anticipated that these will soon not be required, once [CWG Issue
2749](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#2749) is
implemented.  Lastly, the `constexpr` specifier is of course also added
throughout; via `_GLIBCXX26_CONSTEXPR`.

The following `git` command shows the files which differ between the
`constexpr` and `master` branches. The `master` branch is created from an
unmodified `include` directory, obtained after install of a recent GCC build.

```
$ git diff --stat master constexpr -- :/include
 include/c++/17.0.0/atomic                   |   5 +
 include/c++/17.0.0/bits/allocated_ptr.h     |   7 +
 include/c++/17.0.0/bits/atomic_base.h       | 139 +++++++++++++---
 include/c++/17.0.0/bits/out_ptr.h           | 134 +++++++++++++++-
 include/c++/17.0.0/bits/shared_ptr.h        | 238 +++++++++++++++++++++++++++-
 include/c++/17.0.0/bits/shared_ptr_atomic.h | 135 ++++++++++++++++
 include/c++/17.0.0/bits/shared_ptr_base.h   | 207 ++++++++++++++++++++++++
 include/c++/17.0.0/bits/stl_function.h      |   4 +
 include/c++/17.0.0/bits/unique_ptr.h        |   1 +
 include/c++/17.0.0/bits/version.h           |   9 +-
 include/c++/17.0.0/compare                  |  11 ++
 include/c++/17.0.0/ext/atomicity.h          |  10 ++
 12 files changed, 877 insertions(+), 23 deletions(-)
```

(`bits/exception.h`, modified in earlier versions of this work, is no longer
touched: GCC 17's libstdc++ already provides a `constexpr` `std::exception`.)
