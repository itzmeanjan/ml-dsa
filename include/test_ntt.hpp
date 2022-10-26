#pragma once
#include "ntt.hpp"
#include <cassert>
#include <cstring>

// Test functional correctness of Dilithium PQC suite implementation
namespace test_dilithium {

// Ensure functional correctness of (inverse) NTT implementation for degree-255
// polynomial over Z_q | q = 2^23 - 2^13 + 1, using following rule
//
// f -> random polynomial
// f' = ntt(f)
// f'' = intt(f')
//
// assert(f == f'')
static void
test_ntt_intt()
{
  constexpr size_t poly_len = sizeof(ff::ff_t) * ntt::N;

  ff::ff_t* poly_a = static_cast<ff::ff_t*>(std::malloc(poly_len));
  ff::ff_t* poly_b = static_cast<ff::ff_t*>(std::malloc(poly_len));

  for (size_t i = 0; i < ntt::N; i++) {
    poly_a[i] = ff::ff_t::random();
  }

  std::memcpy(poly_b, poly_a, poly_len);

  ntt::ntt(poly_b);
  ntt::intt(poly_b);

  bool flg = true;

  for (size_t i = 0; i < ntt::N; i++) {
    flg &= (poly_a[i] == poly_b[i]);
  }

  std::free(poly_a);
  std::free(poly_b);

  assert(flg);
}

}
