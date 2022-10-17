#include "test_dilithium.hpp"
#include <iostream>

int
main()
{
  test_dilithium::test_field_ops();
  std::cout << "[test] Dilithium prime field arithmetic" << std::endl;

  test_dilithium::test_ntt_intt();
  std::cout << "[test] (i)NTT over degree-255 polynomial" << std::endl;

  return EXIT_SUCCESS;
}
