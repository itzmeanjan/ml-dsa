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

  test_dilithium::test_decompose<((ff::Q - 1u) / 88u) << 1, 77u>();
  test_dilithium::test_decompose<((ff::Q - 1u) / 88u) << 1, 1321u>();
  test_dilithium::test_decompose<((ff::Q - 1u) / 32u) << 1, 997u>();
  test_dilithium::test_decompose<((ff::Q - 1u) / 32u) << 1, 1981u>();
  std::cout << "[test] Making and using of hint bits using Decompose\n";

  return EXIT_SUCCESS;
}
