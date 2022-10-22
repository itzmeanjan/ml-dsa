#include "test_dilithium.hpp"
#include <iostream>

int
main()
{
  test_dilithium::test_field_ops();
  std::cout << "[test] Dilithium prime field arithmetic\n";

  test_dilithium::test_ntt_intt();
  std::cout << "[test] (i)NTT over degree-255 polynomial\n";

  test_dilithium::test_power2round();
  std::cout << "[test] Extraction of high, low order bits using Power2Round\n";

  return EXIT_SUCCESS;
}
