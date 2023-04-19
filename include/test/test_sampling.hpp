#pragma once
#include "prng.hpp"
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
  field::zq_t poly[ntt::N]{};

  prng::prng_t prng;

  prng.read(seed, sizeof(seed));
  sampling::sample_in_ball<τ>(seed, poly);

  field::zq_t sqrd_norm{ 0u };

  for (size_t i = 0; i < ntt::N; i++) {
    sqrd_norm = sqrd_norm + (poly[i] * poly[i]);
  }

  assert(sqrd_norm == field::zq_t{ τ });
}

}
