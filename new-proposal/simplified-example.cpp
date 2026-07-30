#include <type_traits>
#include <cassert>

static_assert(__cpp_lib_is_within_lifetime==202306L);

struct smart_int
{
  union { int* p_; void* v_; };

  constexpr  smart_int() : p_{} {}
  constexpr ~smart_int() { delete get(); }

  constexpr operator  int**() noexcept // const (let's keep the example simple)
  {
    if consteval {
      if (!std::is_within_lifetime(&p_))
        p_ = static_cast<int*>(v_);
    }
    return &p_;
  }

  constexpr operator void**() noexcept // const (let's keep the example simple)
  {
    if consteval {
      if (!std::is_within_lifetime(&v_))
        v_ = p_;
    }
    return &v_;
  }

  constexpr int* get() { return *static_cast<int**>(*this); }
};

constexpr bool test()
{
  auto f = []( int**  pp) {  *pp = new int{1}; };
  auto v = [](void** vpp) { *vpp = new int{2}; };

  smart_int si1, si2;
  f(si1);
  v(si2);
  return *si1.get() == 1 && *si2.get() == 2;
}

int main()
{
  static_assert(test());
  assert(test());
  return 0;
}
