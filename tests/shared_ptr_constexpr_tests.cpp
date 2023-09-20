#include <cassert>
#include <memory>
#include <iostream>
#include <tuple>
#define VERIFY assert
#include "testsuite_allocator.h"

template <template <typename...> typename U>
constexpr bool constexpr_mem_test() {
  // Construction
  U<int> v_int(new int(2));
  if (!v_int || *v_int != 2)
    return false;

  // Assign
  v_int = U<int>(new int(5));
  if (!v_int || *v_int != 5)
    return false;

  // Move
  auto v_int_moved = std::move(v_int);
  if (v_int || !v_int_moved || *v_int_moved != 5)
    return false;

  // Destruction
  bool some_bool = true;
  {
    struct destroy_me_t {
      bool &ref;
      constexpr ~destroy_me_t() { ref = false; }
    };
    destroy_me_t o{some_bool};
  }
  if (some_bool)
    return false;

  // operator->
  {
    struct dummy_t {
      int val;
      constexpr int foo() const { return val; }
    };

    U<dummy_t> dummy_ptr(new dummy_t{42});
    if (dummy_ptr->foo() != 42)
      return false;
  }

  return true;
}

// make_unique is already in libstdc++ for C++23
constexpr bool make_unique_test() {
  // Virtual polymorphism

  struct a_t {
    constexpr virtual int foo() { return 0; }
    constexpr virtual ~a_t() = default;
  };

  struct b_t : a_t {
    constexpr int foo() override { return 1; }
  };

  std::unique_ptr<a_t> a_v = std::make_unique<a_t>();
  std::unique_ptr<a_t> b_v = std::make_unique<b_t>();

  if (a_v->foo() != 0 || b_v->foo() != 1)
    return false;

  // Forwarding

  class m_t {
    int v_;

  public:
    constexpr m_t(int &&v) : v_(v) {}
    constexpr int get_v() { return v_; }
  };

  auto m_v = std::make_unique<m_t>(2);
  if (m_v->get_v() != 2)
    return false;

  bool b{true};
  std::unique_ptr<int> up1 = std::make_unique_for_overwrite<int>();
  *up1 = 2;
  b = b && ( *up1 == 2 );
  std::unique_ptr<int[]> up2 = std::make_unique_for_overwrite<int[]>(2);
  up2[0] = 1;
  up2[1] = 2;
  b = b && ( up2[0] == 1 ) && ( up2[1] == 2 );
  // No. arrays of known bounds; explicitly deleted by specification.
  // std::make_unique_for_overwrite<int[2]>(); // No equivalent for shared_ptr

  return true;
}

constexpr bool make_shared_test() {
  // Virtual polymorphism

  struct a_t {
    constexpr virtual int foo() { return 0; }
    constexpr virtual ~a_t() = default;
  };

  struct b_t : a_t {
    constexpr int foo() override { return 1; }
  };

  std::shared_ptr<a_t> a_v = std::make_shared<a_t>();
  std::shared_ptr<a_t> b_v = std::make_shared<b_t>();

  if (a_v->foo() != 0 || b_v->foo() != 1)
    return false;

  // Forwarding

  class m_t {
    int v_;

  public:
    constexpr m_t(int &&v) : v_(v) {}
    constexpr int get_v() { return v_; }
  };

  auto m_v = std::make_shared<m_t>(2);
  if (m_v->get_v() != 2)
    return false;

  std::shared_ptr<int[48]> p2 = std::make_shared<int[48]>();
  if (p2[47] != 0)
    return false;

  std::shared_ptr<int[]> p3 = std::make_shared<int[]>(4);
  if (p3[0]!=0 || p3[1]!=0 || p3[2]!=0 || p3[3]!=0)
    return false;

  std::remove_extent_t<int[4]> u1{42};
  std::shared_ptr<int[4]> p4 = std::make_shared<int[4]>(u1);
  if (p4[0]!=42 || p4[1]!=42 || p4[2]!=42 || p4[3]!=42)
    return false;

  std::remove_extent_t<int[]> u2{42};
  std::shared_ptr<int[]> p5 = std::make_shared<int[]>(4, u2);
  if (p5[0]!=42 || p5[1]!=42 || p5[2]!=42 || p5[3]!=42)
    return false;

  bool b{true};

  std::shared_ptr<int> sp1 = std::make_shared_for_overwrite<int>();
  *sp1 = 2;
  b = b && ( *sp1 == 2 );

  std::shared_ptr<int[]> sp2 = std::make_shared_for_overwrite<int[]>(2);
  sp2[0] = 1;
  sp2[1] = 2;
  b = b && ( sp2[0] == 1 ) && ( sp2[1] == 2 );

  struct NonTriv
  {
    int init = 0xbb;
    int uninit;
  };

  std::shared_ptr<NonTriv> a_ = std::make_shared_for_overwrite<NonTriv>();
  b = b && ( a_->init == 0xbb );

  std::shared_ptr<NonTriv[]> b_ = std::make_shared_for_overwrite<NonTriv[2]>();
  b = b && ( b_[1].init == 0xbb );

  std::shared_ptr<NonTriv[]> c_ = std::make_shared_for_overwrite<NonTriv[]>(2);
  b = b && ( c_[1].init == 0xbb );

  return b;
}

constexpr void int_array_deleter(int *p) { delete[] p; }

template <template <typename...> typename St> constexpr bool shared_ptr_test() {
  St<int> sp1{new int{123}};
  auto sp2 = sp1;
  bool b1 =
      123 == *sp1 && 123 == *sp1.get() && 123 == *sp2 && 123 == *sp2.get();
  b1 = b1 && 2 == sp1.use_count() && 2 == sp2.use_count();
  b1 = b1 && nullptr == get_deleter<void>(sp1);

  St<int[]> spa1{new int[4]{1, 2, 3, 4}};
  auto spa2 = spa1;
  bool b2 = 1 == spa1[0] && 2 == spa1[1] && 3 == spa1[2] && 4 == spa1[3];
  bool b3 = 1 == spa2[0] && 2 == spa2[1] && 3 == spa2[2] && 4 == spa2[3];

  int i{41};
  bool b4{false};
  int *p = new int{123};
  {
    struct del {
      constexpr void operator()(int *) { ++i_; }
      int &i_;
    };
    del d1{i};
    St<int> sp3{p, d1};
    auto d2 = get_deleter<del>(sp3); // ok
    b4 = d1.i_ == d2->i_;
    auto d3 = [](int *p) { delete p; };
    auto d3b = [](int *p) { delete p; };
    St<int> sp4{new int, d3};
    b4 = b4 && nullptr != get_deleter<decltype(d3)>(sp4);
    b4 = b4 && nullptr == get_deleter<decltype(d3b)>(sp4); // problem
    St<int[]> sp5{new int[4], int_array_deleter};
    b4 = b4 && nullptr != get_deleter<void (*)(int *)>(sp5);
  }
  bool b5 = 42 == i;
  delete p;

  return b1 && b2 && b3 && b4 && b5;
}

// test the aliasing constructor
template <template <typename...> typename St>
constexpr bool shared_ptr_test2() {
  bool b1{false}, b2{false};
  int i{41};
  {
    St<int[]> sp8{new int[8]{1, 2, 3, 4, 5, 6, 7, 8}};
    St<int[]> sp4{sp8, sp8.get() + 4};
    b1 = 1 == sp8[0] && 5 == sp4[0];

    struct del {
      constexpr void operator()(char *) { ++i_; }
      int &i_;
    };
    del d{i};
    char *p = new char[9]{"abcdefgh"};
    St<char[]> sp8b{p, d};
    St<char[]> sp4b{sp8b, sp8b.get() + 4};
    b2 = 'a' == sp8b[0] && 'e' == sp4b[0];
    delete[] p;
  }
  return b1 && b2 && 42 == i;
}

template <typename T, typename P> constexpr bool shared_ptr_compare_test() {
  bool b = true;
  P p(nullptr), q(nullptr);
  b = b && p == q && !(p != q) && p <= q && p >= q && !(p < q) && !(p > q);

  auto test_arr2 = [&](auto p, auto q) {
    b = b && !(p == q) && p != q && p <= q && !(p >= q) && p < q && !(p > q);
    b = b && !(nullptr == p) && !(p == nullptr);
  };

  {
    T arr[2];
    if constexpr (std::is_pointer_v<P>) { // a raw pointer
      test_arr2(&arr[0], &arr[1]);
    } else {
      P p(&arr[0], [](int *) {}), q(p, p.get() + 1); // null deleter
      test_arr2(p, q);
      swap(p, q);
      p.swap(q);
    }
  }

  {
    T *arr = new T[2];
    if constexpr (std::is_pointer_v<P>) { // a raw pointer
      test_arr2(&arr[0], &arr[1]);
      delete[] arr;
    } else {
      P p(&arr[0]), q(p, p.get() + 1); // non-null deleter
      test_arr2(p, q);
    }
  }

  return b;
}

// Clang has a problem with the constexpr qualifier on the
// _Sp_counted_ptr::_M_dispose member function on line 433 of shared_ptr_base.h

constexpr
bool smalltest()
{
  std::shared_ptr<int> sp0{};
  std::__shared_ptr<int> sp0A{};
  std::shared_ptr<int> sp0B{nullptr};
  std::shared_ptr<std::nullptr_t> sp0C{nullptr};
//  std::__shared_ptr<int> spA1{new int{123}};
  std::shared_ptr<int> sp1{new int{123}};
  return true;
}

struct deleter
{
  int count;
  constexpr deleter() : count(0) { }
  constexpr void operator()(std::nullptr_t) { ++count; }
};

constexpr bool extra_shared_ptr_tests()
{
  bool b{true};
  deleter d;

  std::shared_ptr<int> p1 = nullptr;
  b = b && p1.get() == nullptr && p1.use_count() == 0;

  std::shared_ptr<int> p2(nullptr, std::ref(d));
  b = b && p2.get() == nullptr && p2.use_count() == 1;

  __gnu_test::tracker_allocator<int> a;
  std::shared_ptr<int> p3(nullptr, std::ref(d), a);
  b = b && p3.get() == nullptr && p3.use_count() == 1;

  p3 = nullptr;
  b = b && p3.use_count() == 0 && d.count == 1;

  b = b && a.counter_.get_destruct_count() == a.counter_.get_construct_count();
  b = b && a.counter_.get_deallocation_count() == a.counter_.get_allocation_count();

  // operator[] on unbounded arrays
  const std::size_t arr_size = 10;
  std::shared_ptr<int[]> pis(new int[10]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
  for (std::size_t i = 0; i < arr_size; ++i)
    b = b && i == pis[i];

  // operator[][] on 2D unbounded arrays
  auto alloc2 = std::allocator<int>{}; // int[] and int[][12] also work
  auto del2 = [](int p[][12]) { delete [] p; }; // int* will fail
  // int data[][12] = new int[3][12]{}; // no
  int (*data)[12] = new int[3][12]{};
  std::shared_ptr<int[][12]> pis2(data, del2, alloc2);
  pis2[0][0] = 1; pis2[1][0] = 2; pis2[2][0] = 3; // init
  b = b && pis2[0][0] == 1 && pis2[0][1] == 0;

  return b;
}

constexpr bool test_reset()
{
  bool b{true};
  struct A { };
  struct B : A { };
  struct D
  {
    constexpr void operator()(B* p) { delete p; ++delete_count; }
    long& delete_count;
  };

  long delete_count = 0;
  D del{delete_count};

  A * const a1 = new A;
  std::shared_ptr<A> p1a(a1);
  std::shared_ptr<A> p2a(p1a);
  p1a.reset();
  b = b && p1a.get() == 0;
  b = b && p2a.get() == a1;

  A * const a2 = new A;
  B * const b1 = new B;
  std::shared_ptr<A> p1b(a2);
  std::shared_ptr<A> p2b(p1b);
  p1b.reset(b1);
  b = b && p1b.get() == b1;
  b = b && p2b.get() == a2;

  {
    std::shared_ptr<A> p1c;
    p1c.reset(new B, del);
  }
  b = b && del.delete_count == 1;
  return b;
}

constexpr bool test_swap()
{
  bool b{true};
  struct A {};
  A * const a1 = new A;
  A * const a2 = new A;
  std::shared_ptr<A> p1(a1);
  std::shared_ptr<A> p2(a2);
  p1.swap(p2);
  b = b && p1.get() == a2 && p2.get() == a1;
  return b;
}

constexpr bool shared_ptr_assign()
{
  // DR 541. shared_ptr template assignment and void
  std::shared_ptr<void> p;
  p.operator=<void>(p);

  struct A
  {
    constexpr
    A(long& ac, long& ad) : ctor_counta{ac}, dtor_counta{ad} { ++ctor_counta; }
    constexpr virtual ~A() { ++dtor_counta; }
    long& ctor_counta;
    long& dtor_counta;
  };

  struct B : A
  {
    constexpr B(long& ac, long& ad, long& bc, long& bd)
      : ctor_countb{bc}, dtor_countb{bd}, A{ac,ad} { ++ctor_countb; }
    constexpr virtual ~B() { ++dtor_countb; }
    long& ctor_countb;
    long& dtor_countb;
  };

  bool b{true};
  long ac = 0, ad = 0, bc = 0, bd = 0; // outside the scope below - keeps asan happy
  {
    std::shared_ptr<A> a;

    a = std::shared_ptr<A>();
    b = b && a.get() == 0 && ac == 0 && ad == 0 && bc == 0 && bd == 0;

    a = std::shared_ptr<A>(new A{ac,ad});
    b = b && a.get() != 0 && ac == 1 && ad == 0 && bc == 0 && bd == 0;

    a = std::shared_ptr<B>(new B{ac,ad,bc,bd});
    b = b && a.get() != 0 && ac == 2 && ad == 1 && bc == 1 && bd == 0;
  }

  return b;
}

constexpr bool owner_before_test()
{
  struct A
  {
    int i;
    constexpr virtual ~A() { }
  };

  struct B : A
  {
  };

  bool b{true};
  // test empty shared_ptrs compare equivalent
  std::shared_ptr<A> p1;
  std::shared_ptr<B> p2;
  b = b && ( !p1.owner_before(p2) && !p2.owner_before(p1) );

  // Construction from pointer
  std::shared_ptr<A> a0;
  std::shared_ptr<A> a1(new A);
#if CONSTEXPR_FRIENDLY_COMPARISON
  // need an updated std::less
  b = b && (   a1.owner_before(a0) || a0.owner_before(a1) );
  b = b && ( !(a1.owner_before(a0) && a0.owner_before(a1)) );
#endif

  std::shared_ptr<B> b1(new B);
#if CONSTEXPR_FRIENDLY_COMPARISON
  // need an updated std::less
  b = b && (   a1.owner_before(b1) || b1.owner_before(a1) );
  b = b && ( !(a1.owner_before(b1) && b1.owner_before(a1)) );
#endif

  std::shared_ptr<A> a2(a1);
  b = b && ( !a1.owner_before(a2) && !a2.owner_before(a1) );
  a2 = b1;
  b = b && ( !b1.owner_before(a2) && !a2.owner_before(b1) );

  // these need only be run once
  static_assert( noexcept(a1.owner_before(a0)) );
  static_assert( noexcept(a1.owner_before(b1)) );
  static_assert( noexcept(b1.owner_before(a1)) );

  // Aliasing
  std::shared_ptr<A>   p3(new A());
  std::shared_ptr<int> p4(p3, &p3->i);
  b = b && ( !p3.owner_before(p4) && !p4.owner_before(p3) );

  return b;
}

constexpr bool allocate_shared_tests()
{
  bool b{true};

  {        // the initial idea, leading to cest_allocate_shared
    std::allocator<int> alloc;
    using alloc_traits_t = std::allocator_traits<decltype(alloc)>;
    auto del = [alloc](int *p) mutable {
      alloc_traits_t::destroy(alloc, p);
      alloc_traits_t::deallocate(alloc, p, 1);
    };
    auto p  = alloc_traits_t::allocate(alloc, 1);
    //try {
      alloc_traits_t::construct(alloc, std::addressof(*p), 42);
      std::shared_ptr<int> sp_int{p, del, alloc};
    //} catch (...) {
    //  alloc_traits_t::deallocate(alloc, p, 1);
    //  throw;
    //}
    b = b && *sp_int == 42;
  }

  std::shared_ptr<int> sp0a =
    std::allocate_shared<int>(std::allocator<int>{});
  b = b && ( *sp0a == 0 );

  // same overload of allocate_shared as above
  std::shared_ptr<int> sp0a2 =
    std::allocate_shared<int>(std::allocator<int>{}, 42);
  b = b && ( *sp0a2 == 42 );

  std::shared_ptr<int[]> sp0b =
    std::allocate_shared<int[]>(std::allocator<int>{}, 2);
  b = b && ( sp0b[0] == 0 ) && ( sp0b[1] == 0 );

  std::shared_ptr<int[2]> sp0c =
    std::allocate_shared<int[2]>(std::allocator<int>{});
  b = b && ( sp0c[0] == 0 ) && ( sp0c[1] == 0 );

  std::remove_extent_t<int[]> u4{42}; // (4) T is U[]
  std::shared_ptr<int[]> p4 =
    std::allocate_shared<int[]>(std::allocator<int[]>{}, 4, u4);
  if (p4[0]!=42 || p4[1]!=42 || p4[2]!=42 || p4[3]!=42)
    return false;

  std::remove_extent_t<int[4]> u5{42}; // (5) T is U[N]
  std::shared_ptr<int[4]> p5 =
    std::allocate_shared<int[4]>(std::allocator<int[4]>{}, u5);
  if (p5[0]!=42 || p5[1]!=42 || p5[2]!=42 || p5[3]!=42)
    return false;

  std::shared_ptr<int> sp1 =
    std::allocate_shared_for_overwrite<int>(std::allocator<int>{});
  *sp1 = 2;
  b = b && ( *sp1 == 2 );

  std::shared_ptr<int[]> sp2 =
    std::allocate_shared_for_overwrite<int[]>(std::allocator<int>{}, 2);
  sp2[0] = 1;
  sp2[1] = 2;
  b = b && ( sp2[0] == 1 ) && ( sp2[1] == 2 );

  struct NonTriv
  {
    int init = 0xbb;
    int uninit;
  };

  std::shared_ptr<NonTriv> a_ =
    std::allocate_shared_for_overwrite<NonTriv>(std::allocator<NonTriv>{});
  b = b && ( a_->init == 0xbb );

  std::shared_ptr<NonTriv[]> b_ =
    std::allocate_shared_for_overwrite<NonTriv[2]>(std::allocator<NonTriv[]>{});
  b = b && ( b_[1].init == 0xbb );

  std::shared_ptr<NonTriv[]> c_ =
    std::allocate_shared_for_overwrite<NonTriv[]>(
      std::allocator<NonTriv[]>{}, 2);
  b = b && ( c_[1].init == 0xbb );

  return b;
}

constexpr bool shared_owner()
{
  bool b{true};

  std::shared_ptr<std::pair<int,int>> pii(new std::pair<int,int>());
  std::shared_ptr<int> pi1(pii, &pii->first);
  b == b && pii.use_count() == 2;

  std::shared_ptr<int> pi2(std::move(pii), &pii->second);
  b = b && pii.use_count() == 0;

  return b;
}

void memory_tests() {
  static_assert(constexpr_mem_test<std::unique_ptr>(),
                "unique_ptr: Tests failed!");
  static_assert(make_unique_test(), "make_unique: Tests failed!");
  static_assert(make_shared_test(), "make_shared: Tests failed!");
  static_assert(smalltest());
  assert(smalltest());
  static_assert(constexpr_mem_test<std::shared_ptr>(),
                "shared_ptr: Tests failed!");
  static_assert(shared_ptr_test<std::shared_ptr>());
  static_assert(shared_ptr_test2<std::shared_ptr>());
  static_assert(shared_ptr_compare_test<int, int *>());
  static_assert(shared_ptr_compare_test<int, std::shared_ptr<int[]>>());

  assert(constexpr_mem_test<std::unique_ptr>());
  assert(constexpr_mem_test<std::shared_ptr>());
  assert(make_unique_test());
  assert(make_shared_test());

  static_assert(std::is_same_v<std::shared_ptr<int>::element_type, int>);
  static_assert(std::is_same_v<std::shared_ptr<int[]>::element_type, int>);
  assert(shared_ptr_test<std::shared_ptr>());
  assert(shared_ptr_test2<std::shared_ptr>());

  assert((shared_ptr_compare_test<int, int *>()));
  assert((shared_ptr_compare_test<int, std::shared_ptr<int[]>>()));

  assert(extra_shared_ptr_tests());
  static_assert(extra_shared_ptr_tests());

  assert(test_reset());
  static_assert(test_reset());

  assert(test_swap());
  static_assert(test_swap());

  assert(shared_ptr_assign());
  static_assert(shared_ptr_assign());

  assert(owner_before_test());
  static_assert(owner_before_test());

  assert(allocate_shared_tests());
  static_assert(allocate_shared_tests());

  assert(shared_owner());
  static_assert(shared_owner());
}

int main(int argc, char *argv[])
{
  static_assert(__cpp_lib_constexpr_shared_ptr);
  memory_tests();
  return 0;
}
