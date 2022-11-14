#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <random>

// Prime field arithmetic over Z_q, for Dilithium PQC s.t. Q = 2^23 - 2^13 + 1
namespace ff {

// Dilithium Prime Field Modulus
constexpr uint32_t Q = (1u << 23) - (1u << 13) + 1u;

// Precomputed Barrett Reduction Constant
//
// Note,
//
// k = ceil(log2(Q)) = 23
// r = floor((1 << 2k) / Q) = 8396807
//
// See https://www.nayuki.io/page/barrett-reduction-algorithm for more.
constexpr uint32_t R = 8396807;

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
    const ff_t t0 = -rhs;
    return *this + t0;
  }

  // Negation over prime field Z_q | q = 2^23 - 2^13 + 1
  constexpr ff_t operator-() const
  {
    const uint32_t tmp = Q - this->v;
    return ff_t{ tmp };
  }

  // Multiplication over prime field Z_q | q = 2^23 - 2^13 + 1
  //
  // Note, after multiplying two 23 -bit numbers, resulting into a 46 -bit
  // number, it is reduced to Z_q using Barrett reduction algorithm, which
  // avoids division by any value which is not a power of 2.
  //
  // See https://www.nayuki.io/page/barrett-reduction-algorithm for Barrett
  // reduction algorithm
  constexpr ff_t operator*(const ff_t& rhs) const
  {
    const uint64_t t0 = static_cast<uint64_t>(this->v);
    const uint64_t t1 = static_cast<uint64_t>(rhs.v);
    const uint64_t t2 = t0 * t1;

    // operand 0
    const uint64_t t2_hi = t2 >> 32;
    const uint64_t t2_lo = t2 & 0xfffffffful;

    // operand 1
    constexpr uint64_t r_hi = 0ul;
    constexpr uint64_t r_lo = static_cast<uint64_t>(R);

    const uint64_t hi = t2_hi * r_hi;                 // high bits
    const uint64_t mid = t2_hi * r_lo + t2_lo * r_hi; // mid bits
    const uint64_t lo = t2_lo * r_lo;                 // low bits

    const uint64_t mid_hi = mid >> 32;          // high 32 -bits of mid
    const uint64_t mid_lo = mid & 0xfffffffful; // low 32 -bits of mid

    const uint64_t t3 = lo >> 32;
    const uint64_t t4 = t3 + mid_lo;
    const uint64_t carry = t4 >> 32;

    const uint64_t res_hi = hi + mid_hi + carry;
    const uint64_t res_lo = lo + (mid_lo << 32);

    // It must be the case that,
    //
    // if   t3 = t2 * R
    // then ((res_hi << 64) | res_lo) == t3
    //
    // Though result is 128 -bit, with two limbs ( such as res_hi and res_lo ),
    // representing lower & higher 64 -bits, only lower (23 + 23 + 24) -bits are
    // of significance.
    //
    // t0 -> 23 -bit number
    // t1 -> 23 -bit number
    // t2 -> (23 + 23) -bit number      | t2 = t0 * t1
    // R -> 24 -bit number
    // t3 -> (23 + 23 + 24) -bit number | t3 = t2 * R
    //
    // Now we can drop lower 46 -bits of 128 -bit result ( remember, which has
    // only 70 significant bits ) & keep 24 -bits of interest, in res, see
    // below.

    const uint64_t res = ((res_hi & 0x3ful) << 18) | (res_lo >> 46);
    const uint64_t t5 = res * static_cast<uint64_t>(Q);
    const uint32_t t6 = static_cast<uint32_t>(t2 - t5);

    const bool flg = t6 >= Q;
    const uint32_t t7 = t6 - flg * Q;

    return ff_t{ t7 };
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
    ff_t res = br[n & 0b1ul];

    const size_t zeros = std::countl_zero(n);
    const size_t till = 64ul - zeros;

    for (size_t i = 1; i < till; i++) {
      base = base * base;

      const ff_t br[]{ ff_t{ 1u }, base };
      res = res * br[(n >> i) & 0b1ul];
    }

    return res;
  }

  // Equality check between two field elements ∈ Z_q | q = 2^23 - 2^13 + 1
  constexpr bool operator==(const ff_t& rhs) const
  {
    return !static_cast<bool>(this->v ^ rhs.v);
  }

  // Non-equality check between two field elements ∈ Z_q | q = 2^23 - 2^13 + 1
  constexpr bool operator!=(const ff_t& rhs) const
  {
    return static_cast<bool>(this->v ^ rhs.v);
  }

  // Greater than operator applied to elements ∈ Z_q | q = 2^23 - 2^13 + 1
  constexpr bool operator>(const ff_t& rhs) const { return this->v > rhs.v; }

  // Greater than equal operator applied to elements ∈ Z_q | q = 2^23 - 2^13 + 1
  constexpr bool operator>=(const ff_t& rhs) const { return this->v >= rhs.v; }

  // Lesser than operator applied to elements ∈ Z_q | q = 2^23 - 2^13 + 1
  constexpr bool operator<(const ff_t& rhs) const { return this->v < rhs.v; }

  // Lesser than equal operator applied to elements ∈ Z_q | q = 2^23 - 2^13 + 1
  constexpr bool operator<=(const ff_t& rhs) const { return this->v <= rhs.v; }

  // Shifts operand ∈ Z_q, leftwards by l bit positions | q = 2^23 - 2^13 + 1
  constexpr ff_t operator<<(const size_t l) const
  {
    return ff_t{ this->v << l };
  }

  // Generate a random field element ∈ Z_q | q = 2^23 - 2^13 + 1
  static ff_t random()
  {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint32_t> dis{ 0, Q - 1ul };

    return ff_t{ dis(gen) };
  }

  // Writes element of Z_q to output stream | q = 2^23 - 2^13 + 1
  friend std::ostream& operator<<(std::ostream& os, const ff_t& elm);
};

std::ostream&
operator<<(std::ostream& os, const ff_t& elm)
{
  return os << "Z_q(" << elm.v << ", " << Q << ")";
}

}
