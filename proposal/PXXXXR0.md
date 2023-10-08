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
