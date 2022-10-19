#pragma once
#include "poly.hpp"

// Utility functions for Dilithium Post-Quantum Digital Signature Algorithm
namespace dilithium_utils {

// Applies NTT on a vector ( of dimension k x 1 ) of degree-255 polynomials
template<const size_t k>
static void
polyvec_ntt(ff::ff_t* const __restrict vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    ntt::ntt(vec + off);
  }
}

// Applies iNTT on a vector ( of dimension k x 1 ) of degree-255 polynomials
template<const size_t k>
static void
polyvec_intt(ff::ff_t* const __restrict vec)
{
  for (size_t i = 0; i < k; i++) {
    const size_t off = i * ntt::N;
    ntt::intt(vec + off);
  }
}

}
