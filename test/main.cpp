#include "test_dilithium.hpp"
#include <iostream>

int
main()
{
  test_dilithium::test_field_ops();
  std::cout << "[test] Dilithium prime field arithmetic" << std::endl;

  return EXIT_SUCCESS;
}
