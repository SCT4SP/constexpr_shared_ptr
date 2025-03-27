### `constexpr std::shared_ptr` and friends

R5 [https://isocpp.org/files/papers/P3037R5.pdf](https://isocpp.org/files/papers/P3037R5.pdf)

R4 [https://isocpp.org/files/papers/P3037R4.pdf](https://isocpp.org/files/papers/P3037R4.pdf)

R3 [https://isocpp.org/files/papers/P3037R3.pdf](https://isocpp.org/files/papers/P3037R3.pdf)

R2 [https://isocpp.org/files/papers/P3037R2.pdf](https://isocpp.org/files/papers/P3037R2.pdf)

R1 [https://isocpp.org/files/papers/P3037R1.pdf](https://isocpp.org/files/papers/P3037R1.pdf)

R0 [https://isocpp.org/files/papers/P3037R0.pdf](https://isocpp.org/files/papers/P3037R0.pdf)

The default `constexpr` branch contains a `constexpr` implementation of
`std::shared_ptr` for potential inclusion in C++26. The code is based on the
`std::shared_ptr` implementation in GCC's libstdc++; and many of the
`constexpr` unit tests included here are derived from the libstdc++ testsuite.
The implementation is presented as a modification of a pre-installed set
of libstdc++ header files; and the `include` directory provided here can
potentially be used, along with a recent version of GCC (installed here at
`/opt/gcc-latest/`), or Clang, via:

```
$ $CXX -Wl,-rpath,"/opt/gcc-latest/lib64:$LD_LIBRARY_PATH" -L /opt/gcc-latest/lib64 -std=c++26 -Winvalid-constexpr -fsanitize=address -I $PWD/include/c++/15.0.1 -I $PWD/include/c++/15.0.1/x86_64-pc-linux-gnu main.cpp
```

Most of the changes required for this implementation are within two files:

```
include/c++/15.0.1/bits/shared_ptr.h
include/c++/15.0.1/bits/shared_ptr_base.h
include/c++/15.0.1/bits/atomic_base.h
```

The foot of `include/c++/15.0.1/bits/version.h` includes definition of a
feature macro `__cpp_lib_constexpr_shared_ptr` (set to `202503L`, i.e. the
current month); assuming the -std=c++26 flag has been set.

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
$ git diff --name-only master constexpr -- include
include/c++/15.0.1/bits/allocated_ptr.h
include/c++/15.0.1/bits/atomic_base.h
include/c++/15.0.1/bits/exception.h
include/c++/15.0.1/bits/shared_ptr.h
include/c++/15.0.1/bits/shared_ptr_base.h
include/c++/15.0.1/bits/stl_function.h
include/c++/15.0.1/bits/unique_ptr.h
include/c++/15.0.1/bits/version.h
include/c++/15.0.1/compare
include/c++/15.0.1/ext/atomicity.h
```
