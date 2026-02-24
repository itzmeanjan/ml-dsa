#include "ml_dsa/internals/poly/sampling.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  if (size < 1) {
    return -1;
  }

  const uint8_t selector = data[0] % 4;
  const uint8_t* payload = data + 1;
  const size_t psize = size - 1;

  if (selector == 0) { // expand_a
    if (psize < 32)
      return -1;
    std::array<ml_dsa_field::zq_t, 4 * 4 * ml_dsa_ntt::N> mat; // Use k=4, l=4 as a test case
    ml_dsa_sampling::expand_a<4, 4>(std::span<const uint8_t, 32>(payload, 32), mat);
  } else if (selector == 1) { // expand_s
    if (psize < 64)
      return -1;
    std::array<ml_dsa_field::zq_t, 4 * ml_dsa_ntt::N> vec; // Use k=4, eta=2
    ml_dsa_sampling::expand_s<2, 4, 0>(std::span<const uint8_t, 64>(payload, 64), vec);
  } else if (selector == 2) { // expand_mask
    if (psize < 64)
      return -1;
    std::array<ml_dsa_field::zq_t, 4 * ml_dsa_ntt::N> vec; // Use l=4, gamma1=1^17
    ml_dsa_sampling::expand_mask<(1 << 17), 4>(std::span<const uint8_t, 64>(payload, 64), 0, vec);
  } else if (selector == 3) { // sample_in_ball
    if (psize < 32)
      return -1; // Use lambda=128
    std::array<ml_dsa_field::zq_t, ml_dsa_ntt::N> poly;
    ml_dsa_sampling::sample_in_ball<39, 128>(std::span<const uint8_t, 32>(payload, 32), poly);
  }

  return 0;
}
