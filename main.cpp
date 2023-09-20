#include <memory>

int main(int argc, char *argv[])
{
  static_assert(__cpp_lib_constexpr_shared_ptr);
  static_assert((std::shared_ptr<int>{}, true));
  return 0;
}
