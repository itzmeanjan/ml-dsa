#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

// Prime field arithmetic over Z_q, for Dilithium PQC s.t. Q = 2^23 - 2^13 + 1
namespace ff {

// Dilithium Prime Field Modulus
constexpr uint32_t Q = (1u << 23) - (1u << 13) + 1u;

// Extended GCD algorithm for computing multiplicative inverse of prime ( = Q )
// field element
//
// Taken from
// https://github.com/itzmeanjan/kyber/blob/3cd41a5/include/ff.hpp#L49-L82
static constexpr std::array<int32_t, 3>
xgcd(const uint32_t x, const uint32_t y)
{
  int32_t old_r = static_cast<int32_t>(x), r = static_cast<int32_t>(y);
  int32_t old_s = 1, s = 0;
  int32_t old_t = 0, t = 1;

  while (r != 0) {
    int32_t quotient = old_r / r;
    int32_t tmp = 0;

    tmp = old_r;
    old_r = r;
    r = tmp - quotient * r;

    tmp = old_s;
    old_s = s;
    s = tmp - quotient * s;

    tmp = old_t;
    old_t = t;
    t = tmp - quotient * t;
  }

  return {
    old_s, // a
    old_t, // b
    old_r  // g
  };       // s.t. `ax + by = g`
}

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

  // Negation over prime field Z_q | q = 2^23 - 2^13 + 1
  constexpr ff_t operator-() const
  {
    const uint32_t tmp = Q - this->v;
    return ff_t{ tmp };
  }

  // Multiplication over prime field Z_q | q = 2^23 - 2^13 + 1
  constexpr ff_t operator*(const ff_t& rhs) const
  {
    const uint64_t t0 = static_cast<uint64_t>(this->v);
    const uint64_t t1 = static_cast<uint64_t>(rhs.v);
    const uint64_t t2 = t0 * t1;

    const uint64_t t3 = t2 / Q;
    const uint64_t t4 = t3 * Q;
    const uint32_t t5 = static_cast<uint32_t>(t2 - t4);

    return ff_t{ t5 };
  }

  // Multiplicative inverse over prime field Z_q | q = 2^23 - 2^13 + 1
  //
  // Say input is a & return value of this function is b, then
  //
  // assert (a * b) % q == 1
  //
  // When input a = 0, multiplicative inverse can't be computed, hence return
  // value is 0.
  //
  // Taken from
  // https://github.com/itzmeanjan/kyber/blob/3cd41a5/include/ff.hpp#L190-L216
  constexpr ff_t inv() const
  {
    const bool flg0 = this->v == 0;
    const uint32_t t0 = this->v + flg0 * 1;

    auto res = xgcd(t0, Q);

    const bool flg1 = res[0] < 0;
    const uint32_t t1 = static_cast<uint32_t>(flg1 * Q + res[0]);

    const bool flg2 = t1 >= Q;
    const uint32_t t2 = t1 - flg2 * Q;
    const uint32_t t3 = t2 - flg0 * 1;

    return ff_t{ t3 };
  }

  // Division over prime field Z_q | q = 2^23 - 2^13 + 1
  constexpr ff_t operator/(const ff_t& rhs) const
  {
    return (*this) * rhs.inv();
  }

  // Raises field element to N -th power, using exponentiation by repeated
  // squaring rule
  //
  // Taken from
  // https://github.com/itzmeanjan/kyber/blob/3cd41a5/include/ff.hpp#L224-L246
  constexpr ff_t operator^(const size_t n) const
  {
    ff_t base = *this;

    const ff_t br[]{ ff_t{ 1u }, base };
    ff_t r = br[n & 0b1ul];

    const size_t zeros = std::countl_zero(n);
    const size_t till = 64ul - zeros;

    for (size_t i = 1; i < till; i++) {
      base = base * base;

      const ff_t br[]{ ff_t{ 1u }, base };
      r = r * br[(n >> i) & 0b1ul];
    }

    return r;
  }
};

}
