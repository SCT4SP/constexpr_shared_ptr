---
title: "`constexpr std::shared_ptr`"
document: PXXXXR0
date: 2023-10-01
audience: Library Evolution Working Group (LEWG)
author:
  - name: A B
    email: <a@b.com>
  - name: C D
    email: <c@d.com>
toc: true
toc-depth: 4
---

\pagebreak

# Introduction

Since the adoption of [@P0784R7] in C++20, constant expressions can include
dynamic memory allocation; yet support for smart pointers extends only to
`std::unique_ptr` (since [@P2273R3] in C++23). As at runtime, smart pointers
can encourage hygienic memory management during constant evaluation; and
with no remaining technical obstacles, parity between runtime and compile-time
support for smart pointers can duly and intuitively reflect the increased
maturity of language support for constant expression evaluation.  We therefore
propose that `std::shared_ptr` and associated classes from [util.sharedptr]
permit `constexpr` evaluation.

# Motivation and Scope

Two proposals recently adopted for C++23 can facilitate a straightforward
implementation of comprehensive `constexpr` support for `std::shared_ptr`:
[@P2738R1] and [@P2448R2]. The former allows the `get_deleter` member function
to operate, given the type erasure required within the `std::shared_ptr` unary
class template. The latter can allow even minor associated classes such as
`std::bad_weak_ptr` to receive `constexpr` qualification, while inheriting from
the currently non-`constexpr` class: `std::exception`. We furthermore propose
that the relational operators of `std::unique_ptr`, which can legally operate on
pointers originating from a single allocation during constant evaluation,
should also adopt the `constexpr` specifier.

As with C++23 `constexpr` support for `std::unique_ptr`, bumping the value
`__cpp_lib_constexpr_memory` is our requested feature macro change; yet in the
discussion and implementation presented here, we adopt the macro
`__cpp_lib_constexpr_shared_ptr`. We also use the `_GLIBCXX26_CONSTEXPR` macro
in place of the literal `constexpr` keyword to ensure the specifier only
applies when the `-std=c++26` flag is enabled.

We below elaborate on points which go beyond the simple addition of the
`constexpr` specifier to the relevant member functions.

## Atomic Operations

`std::shared_ptr` can operate within a multithreaded runtime environment; and a
number of its member functions use atomic functions to ensure that shared state
is updated correctly. Constant expressions must currently be evaluated by a
single thread. A `constexpr` `std::shared_ptr` implementation can engage with
the `constexpr`-friendly support for single-threaded evaluation available in
atomic function definitions within standard library implementations. For
example, in libstdc++'s interface to atomic functions, the
`__is_single_threaded` function, which controls execution of both
`__exchange_and_add_dispatch` and `__atomic_add_dispatch` within the
`ext/atomicity.h` header file, can be changed to start as follows:

```cpp
_GLIBCXX26_CONSTEXPR
__attribute__((__always_inline__))
inline bool
__is_single_threaded() _GLIBCXX_NOTHROW
{
#ifdef __cpp_lib_constexpr_shared_ptr
  if (std::is_constant_evaluated())
    return true;
#endif
  // ... 7 more lines here
}
```

Built-in GCC atomic functions such as `__atomic_load_n` are also used within
libstdc++'s implementation of `std::shared_ptr`. These could similarly be
updated to account for a `constexpr` single-threaded execution environment
within the compiler. The approach taken within our own implementation is a
local one; eliding the call to the atomic function through the predication of
`std::is_constant_evaluated` (or `if consteval`). For example, here is an
updated function from `bits/shared_ptr_base.h`, used by
`std::shared_ptr::use_count()` and elsewhere:

```cpp
_GLIBCXX26_CONSTEXPR
long
_M_get_use_count() const noexcept
{
#ifdef __cpp_lib_constexpr_shared_ptr
  return std::is_constant_evaluated()
           ? _M_use_count
           : __atomic_load_n(&_M_use_count, __ATOMIC_RELAXED);
#else
  return __atomic_load_n(&_M_use_count, __ATOMIC_RELAXED);
#endif
}
```
## Two Memory Allocations

Unlike `std::unique_ptr`, a `std::shared_ptr` must store not only the managed
object, but also the type-erased deleter and allocator, as well as the number
of `std::shared_ptr`s and `std::weak_ptr`s which own or refer to the managed
object. This information is managed as part of a dynamically allocated object
referred to as the *control block*.

Existing runtime implementations of `make_shared`, `allocate_shared`,
`make_shared_for_overwrite`, and `allocate_shared_for_overwrite`, allocate
memory for both the control block, *and* the managed object, from a single
dynamic memory allocation; via `std::reinterpret_cast`.  This practise aligns
with a remark at clause 7.1 of [util.smartptr.shared.create] quoted below:

  - [7.1]{.pnum} Implementations should perform no more than one memory allocation.
  - [*Note 1*: This provides efficiency equivalent to an intrusive smart pointer.  — *end note*]

As `reinterpret_cast` is not permitted within a constant expression, an
alternative approach is required for `make_shared`, `allocate_shared`,
`make_shared_for_overwrite`, and `allocate_shared_for_overwrite`.  A
straightforward solution is to create the object first, and pass its address to
the appropriate `std::shared_ptr` constructor. Considering the control block,
this approach amounts to two dynamic memory allocations; albeit at
compile-time. Assuming that the runtime implementation need not change, the
remark quoted above could either be removed, or changed to "Implementations
should perform no more than one runtime memory allocation."

# Impact on the Standard

This proposal is a pure library extension, and does not require any new language features.

# Implementation

# Proposed Wording

# Acknowledgements

Thanks to all of the following:

  - (In alphabetical order by last name) Thiago Macieira, Arthur O'Dwyer,
    and everyone else who contributed to the online forum discussions.

\pagebreak

---
references:
  - id: P0784R7
    citation-label: P0784R7
    title: "More constexpr containers"
    author:
      family: Dimov
      given: Peter
      family: Dionne
      given: Louis
      family: Ranns
      given: Nina
      family: Smith
      given: Richard
      family: Vandevoorde
      given: Daveed
    issued:
      year: 2019
    URL: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0784r7.html
  - id: P2738R1
    citation-label: P2738R1
    title: "`constexpr` cast from `void*`: towards `constexpr` type-erasure"
    author:
      family: Jabot
      given: Corentin
      family: Ledger
      given: David
    issued:
      year: 2023
    URL: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2738r1.pdf
  - id: P2448R2
    citation-label: P2448R2
    title: "Relaxing some `constexpr` restrictions"
    author:
      family: Revzin
      given: Barry
    issued:
      year: 2022
    URL: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2448r2.html
  - id: P2273R3
    citation-label: P2273R3
    title: "Making `std::unique_ptr` constexpr"
    author:
      family: Fertig
      given: Andreas
    issued:
      year: 2021
    URL: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2273r3.pdf
---
