#include <cstddef>
#include <cstdint>
#include <cstring>
#include "ml_dsa/internals/math/field.hpp"

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  if (size < 8) {
    return -1;
  }

  uint32_t a_raw, b_raw;
  std::memcpy(&a_raw, data, 4);
  std::memcpy(&b_raw, data + 4, 4);

  ml_dsa_field::zq_t a(a_raw);
  ml_dsa_field::zq_t b(b_raw);

  (void)(a + b);
  (void)(a - b);
  (void)(a * b);
  (void)(-a);
  (void)(a == b);

  return 0;
}
