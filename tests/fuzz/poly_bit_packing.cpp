#include <cstddef>
#include <cstdint>
#include <array>
#include <span>
#include "ml_dsa/internals/poly/bit_packing.hpp"

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  if (size < 1) {
    return -1;
  }

  const uint8_t sbw_selector = data[0] % 8;
  const uint8_t* payload = data + 1;
  const size_t psize = size - 1;

  size_t sbw = 0;
  switch (sbw_selector) {
    case 0: sbw = 3; break;
    case 1: sbw = 4; break;
    case 2: sbw = 6; break;
    case 3: sbw = 10; break;
    case 4: sbw = 13; break;
    case 5: sbw = 18; break;
    case 6: sbw = 20; break;
    default: sbw = 3; break;
  }

  const size_t blen = (ml_dsa_ntt::N * sbw) / 8;
  if (psize < blen) {
    return -1;
  }

  std::array<ml_dsa_field::zq_t, ml_dsa_ntt::N> poly;
  
  // Test decode
  // Switch based on sbw manually since it's a template parameter
  if (sbw == 3) ml_dsa_bit_packing::decode<3>(std::span<const uint8_t, (ml_dsa_ntt::N * 3) / 8>(payload, blen), poly);
  else if (sbw == 4) ml_dsa_bit_packing::decode<4>(std::span<const uint8_t, (ml_dsa_ntt::N * 4) / 8>(payload, blen), poly);
  else if (sbw == 6) ml_dsa_bit_packing::decode<6>(std::span<const uint8_t, (ml_dsa_ntt::N * 6) / 8>(payload, blen), poly);
  else if (sbw == 10) ml_dsa_bit_packing::decode<10>(std::span<const uint8_t, (ml_dsa_ntt::N * 10) / 8>(payload, blen), poly);
  else if (sbw == 13) ml_dsa_bit_packing::decode<13>(std::span<const uint8_t, (ml_dsa_ntt::N * 13) / 8>(payload, blen), poly);
  else if (sbw == 18) ml_dsa_bit_packing::decode<18>(std::span<const uint8_t, (ml_dsa_ntt::N * 18) / 8>(payload, blen), poly);
  else if (sbw == 20) ml_dsa_bit_packing::decode<20>(std::span<const uint8_t, (ml_dsa_ntt::N * 20) / 8>(payload, blen), poly);

  // Test encode
  std::array<uint8_t, (ml_dsa_ntt::N * 20) / 8> arr; // Max possible size
  if (sbw == 3) ml_dsa_bit_packing::encode<3>(poly, std::span<uint8_t, (ml_dsa_ntt::N * 3) / 8>(arr.data(), blen));
  else if (sbw == 4) ml_dsa_bit_packing::encode<4>(poly, std::span<uint8_t, (ml_dsa_ntt::N * 4) / 8>(arr.data(), blen));
  else if (sbw == 6) ml_dsa_bit_packing::encode<6>(poly, std::span<uint8_t, (ml_dsa_ntt::N * 6) / 8>(arr.data(), blen));
  else if (sbw == 10) ml_dsa_bit_packing::encode<10>(poly, std::span<uint8_t, (ml_dsa_ntt::N * 10) / 8>(arr.data(), blen));
  else if (sbw == 13) ml_dsa_bit_packing::encode<13>(poly, std::span<uint8_t, (ml_dsa_ntt::N * 13) / 8>(arr.data(), blen));
  else if (sbw == 18) ml_dsa_bit_packing::encode<18>(poly, std::span<uint8_t, (ml_dsa_ntt::N * 18) / 8>(arr.data(), blen));
  else if (sbw == 20) ml_dsa_bit_packing::encode<20>(poly, std::span<uint8_t, (ml_dsa_ntt::N * 20) / 8>(arr.data(), blen));

  return 0;
}
