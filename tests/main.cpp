#include "tests/test_dilithium.hpp"
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

  test_dilithium::test_decompose<((field::Q - 1u) / 88u) << 1, 77u>();
  test_dilithium::test_decompose<((field::Q - 1u) / 88u) << 1, 1321u>();
  test_dilithium::test_decompose<((field::Q - 1u) / 32u) << 1, 997u>();
  test_dilithium::test_decompose<((field::Q - 1u) / 32u) << 1, 1981u>();
  std::cout << "[test] Making and using of hint bits using Decompose\n";

  test_dilithium::test_sample_in_ball<39u>();
  test_dilithium::test_sample_in_ball<49u>();
  test_dilithium::test_sample_in_ball<60u>();
  std::cout << "[test] Hashing to a ball\n";

  test_dilithium::test_encode_decode<3>();
  test_dilithium::test_encode_decode<4>();
  test_dilithium::test_encode_decode<6>();
  test_dilithium::test_encode_decode<10>();
  test_dilithium::test_encode_decode<13>();
  test_dilithium::test_encode_decode<18>();
  test_dilithium::test_encode_decode<20>();
  std::cout << "[test] Polynomial encoding/ decoding\n";

  test_dilithium::test_encode_decode_hint_bits<4, 80>();
  test_dilithium::test_encode_decode_hint_bits<6, 55>();
  test_dilithium::test_encode_decode_hint_bits<8, 75>();
  std::cout << "[test] Hint bit polynomial encoding/ decoding\n";

  {
    using namespace test_dilithium;

    for (size_t mlen = 1; mlen < 33; mlen++) {
      test_dilithium2_signing(mlen);
      test_dilithium3_signing(mlen);
      test_dilithium5_signing(mlen);
    }

    std::cout << "[test] Dilithium KeyGen -> Signing -> Verification\n";
  }

  test_dilithium::test_dilithium2_signing_kat();
  test_dilithium::test_dilithium3_signing_kat();
  test_dilithium::test_dilithium5_signing_kat();
  std::cout << "[test] Dilithium Known Answer Tests\n";

  return EXIT_SUCCESS;
}
