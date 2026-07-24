#include <memory>

int main(int argc, char *argv[])
{
  static_assert(__cpp_lib_constexpr_memory >= 202506L);
  static_assert((std::shared_ptr<int>{}, true));
  return 0;
}
