The default `constexpr` branch contains a `constexpr` implementation of
`std::shared_ptr` for potential inclusion in C++26. The code is based on the
`std::shared_ptr` implementation in GCC's libstdc++; and many of the
`constexpr` unit tests included here are derived from the libstdc++ testsuite.
The implementation here is presented as a modification of a pre-installed set
of libstdc++ header files; and the `include` directory provided here can
potentially be used, along with a recent version of GCC (installed here at
`/opt/gcc-latest/`), or Clang, via:

```
$CXX -Wl,-rpath,"/opt/gcc-latest/lib64:$LD_LIBRARY_PATH" -L /opt/gcc-latest/lib64 -std=c++26 -Winvalid-constexpr -fsanitize=address -I $PWD/include/c++/14.0.0 -I $PWD/include/c++/14.0.0/x86_64-pc-linux-gnu main.cpp
```

Most of the changes required for this implementation are within two files:

```
include/c++/14.0.0/bits/shared_ptr.h
include/c++/14.0.0/bits/shared_ptr_base.h
```

The foot of `include/c++/14.0.0/bits/version.h` includes definition of a
feature macro `__cpp_lib_constexpr_shared_ptr` (set to `202309L`, i.e. the
current month); assuming the -std=c++26 flag has been set.  A
`_GLIBCXX26_CONSTEXPR` macro (defined in
`include/c++/14.0.0/x86_64-pc-linux-gnu/bits/c++config.h`) is also used
throughout; in place of a `constexpr` literal.

One enabler of `constexpr` `shared_ptr` is the implementation of adopted
proposal P2738 in GCC and Clang; which allows `constexpr` casting from `void*`;
visible in the `get_deleter` implementation. `constexpr` incompatible atomic
operations have been avoided, under the asumption that constant expression
evaluation will be single-threaded (see `_M_add_ref_copy` in
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
2749](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#2749) is
approved.  Lastly, the `constexpr` specifier is of course also added
throughout; via `_GLIBCXX26_CONSTEXPR`.

The following `git` command shows the files which differ between the
`constexpr` and `master` branches. The `master` branch is created from an
unmodified `include` directory, obtained after install of a recent GCC build.

```
git diff --name-only master constexpr -- include
include/c++/14.0.0/bits/allocated_ptr.h
include/c++/14.0.0/bits/shared_ptr.h
include/c++/14.0.0/bits/shared_ptr_base.h
include/c++/14.0.0/bits/stl_function.h
include/c++/14.0.0/bits/unique_ptr.h
include/c++/14.0.0/bits/version.h
include/c++/14.0.0/compare
include/c++/14.0.0/ext/atomicity.h
include/c++/14.0.0/x86_64-pc-linux-gnu/bits/c++config.h
```
