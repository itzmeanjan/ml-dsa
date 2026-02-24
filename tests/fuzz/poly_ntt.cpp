#include "ml_dsa/internals/math/field.hpp"
#include "ml_dsa/internals/poly/ntt.hpp"
#include <cstddef>
#include <cstdint>

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  if (size < ml_dsa_ntt::N * 4) {
    return -1;
  }

  std::array<ml_dsa_field::zq_t, ml_dsa_ntt::N> poly;
  for (size_t i = 0; i < ml_dsa_ntt::N; ++i) {
    uint32_t val;
    std::memcpy(&val, data + i * 4, 4);
    poly[i] = ml_dsa_field::zq_t(val);
  }

  ml_dsa_ntt::ntt(poly);
  ml_dsa_ntt::intt(poly);

  return 0;
}
