The constexpr branch contains a constexpr implementation of `std::shared_ptr`
for potential inclusion in C++26. The code is based on the `std::shared_ptr`
implementation in GCC's libstdc++; and many of the `constexpr` unit tests
included here are derived from the libstdc++ testsuite. The implementation
here is presented as a modification of a pre-installed set of libstdc++ header
files; and the `include` directory provided here can potentially be used,
along with a recent version of GCC  or Clang via:

Most of the changes required are within two files:

```
include/c++/14.0.0/bits/shared_ptr.h
include/c++/14.0.0/bits/shared_ptr_base.h
```

The foot of `include/c++/14.0.0/bits/version.h` includes definition of a
feature macro `__cpp_lib_constexpr_shared_ptr` to `202309L` (i.e. the current
month); assuming the -std=c++26 flag has been set.

One enabler of `constexpr` `shared_ptr` is the implementation of adopted
proposal P2738 in GCC and Clang; which allows `constexpr` casting from `void*`;
visible in the `get_deleter` implementation. `constexpr` incompatible
atomic operations have been avoided, under the asumption that constant
expression evaluation will be single-threaded (see `_M_add_ref_copy` in
`shared_ptr_base.h` for example). Lastly, cest_allocate_shared

```
$ git diff --name-only constexpr master
include/c++/14.0.0/bits/allocated_ptr.h
include/c++/14.0.0/bits/shared_ptr.h
include/c++/14.0.0/bits/shared_ptr_base.h
include/c++/14.0.0/bits/version.h
include/c++/14.0.0/compare
include/c++/14.0.0/x86_64-pc-linux-gnu/bits/c++config.h
```
