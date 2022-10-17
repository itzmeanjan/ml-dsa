#pragma once
#include <cstdint>

// Prime field arithmetic over Z_q, for Dilithium PQC s.t. Q = 2^23 - 2^13 + 1
namespace ff {

// Dilithium Prime Field Modulus
constexpr uint32_t Q = (1u << 23) - (1u << 13) + 1u;

// Dilithium Prime Field element e ∈ [0, Q), with arithmetic operations defined
// & implemented over it.
struct ff_t
{
  uint32_t v = 0u;

  // Construct field element, holding canonical value _v % Q
  inline constexpr ff_t(const uint32_t _v = 0u) { v = _v % Q; }

  // Construct field element, holding canonical value 0
  static inline ff_t zero() { return ff_t{ 0u }; }

  // Construct field element, holding canonical value 1
  static inline ff_t one() { return ff_t{ 1u }; }

  // Addition over prime field Z_q | q = 2^23 - 2^13 + 1
  constexpr ff_t operator+(const ff_t& rhs) const
  {
    const uint32_t t0 = this->v + rhs.v;
    const bool flg = t0 >= Q;
    const uint32_t t1 = t0 - flg * Q;

    return ff_t{ t1 };
  }

  // Subtraction over prime field Z_q | q = 2^23 - 2^13 + 1
  constexpr ff_t operator-(const ff_t& rhs) const
  {
    const uint32_t t0 = (Q + this->v) - rhs.v;
    const bool flg = t0 >= Q;
    const uint32_t t1 = t0 - flg * Q;

    return ff_t{ t1 };
  }
};

}
