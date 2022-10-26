#pragma once
#include "sampling.hpp"
#include "utils.hpp"
#include <cassert>

// Test functional correctness of Dilithium PQC suite implementation
namespace test_dilithium {

// Check whether hashing to a ball routine works as expected or not
template<const uint32_t τ>
static void
test_sample_in_ball()
{
  uint8_t seed[32]{};
  ff::ff_t poly[ntt::N]{};

  dilithium_utils::random_data<uint8_t>(seed, sizeof(seed));
  dilithium_utils::sample_in_ball<τ>(seed, poly);

  ff::ff_t sqrd_norm{ 0u };

  for (size_t i = 0; i < ntt::N; i++) {
    sqrd_norm = sqrd_norm + (poly[i] * poly[i]);
  }

  assert(sqrd_norm == ff::ff_t{ τ });
}

}
