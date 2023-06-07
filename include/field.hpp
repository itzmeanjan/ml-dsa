#pragma once
#include "prng.hpp"
#include <bit>
#include <cstddef>
#include <cstdint>

// Prime field arithmetic over Z_q, for Dilithium PQC s.t. Q = 2^23 - 2^13 + 1
namespace field {

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

// Given a 32 -bit unsigned integer value v, this routine can be used for
// reducing it by modulo prime Q = 2^23 - 2^13 + 1, computing v' ∈ [0, Q),
// without using division/ modulo division operator.
//
// ∀ v ∈ [0, 2^32), mod_reduce(v) == (v % Q) - must hold !
inline constexpr uint32_t
mod_reduce(const uint32_t val)
{
  constexpr uint32_t mask23 = (1u << 23) - 1u;
  constexpr uint32_t mask13 = (1u << 13) - 1u;
  constexpr uint32_t u23_max = mask23;

  const uint32_t hi = val >> 23;
  const uint32_t lo = val & mask23;

  const uint32_t t0 = (hi << 13) - hi;
  const uint32_t t1 = t0 + lo;
  const bool flg0 = t0 > (u23_max - lo);
  const uint32_t t2 = (-static_cast<uint32_t>(flg0)) & mask13;
  const uint32_t t3 = t1 + t2;
  const uint32_t t4 = t3 & mask23;

  const bool flg1 = t4 > Q;
  const uint32_t t5 = (-static_cast<uint32_t>(flg1)) & Q;
  const uint32_t t6 = t4 - t5;

  return t6;
}

// Dilithium Prime Field element e ∈ [0, Q), with arithmetic operations defined
// & implemented over it.
struct zq_t
{
  uint32_t v = 0u;

  inline constexpr zq_t() = default;

  // Constructs field element s.t. input is already prime modulo reduced.
  inline explicit constexpr zq_t(const uint32_t val) { v = val; }

  static inline zq_t zero() { return zq_t(); }

  static inline zq_t one() { return zq_t(1u); }

  // Addition over prime field Z_q | q = 2^23 - 2^13 + 1
  constexpr zq_t operator+(const zq_t& rhs) const
  {
    const uint32_t t0 = this->v + rhs.v;
    const uint32_t mask = (-static_cast<uint32_t>(t0 >= Q));
    const uint32_t t1 = t0 - (mask & Q);

    return zq_t(t1);
  }

  // Negation over prime field Z_q | q = 2^23 - 2^13 + 1
  constexpr zq_t operator-() const
  {
    const uint32_t tmp = Q - this->v;
    return zq_t(tmp);
  }

  // Subtraction over prime field Z_q | q = 2^23 - 2^13 + 1
  constexpr zq_t operator-(const zq_t& rhs) const
  {
    const zq_t t0 = -rhs;
    return *this + t0;
  }

  // Multiplication over prime field Z_q | q = 2^23 - 2^13 + 1
  //
  // Note, after multiplying two 23 -bit numbers, resulting into a 46 -bit
  // number, it is reduced to Z_q using Barrett reduction algorithm, which
  // avoids division by any value which is not a power of 2.
  //
  // See https://www.nayuki.io/page/barrett-reduction-algorithm for Barrett
  // reduction algorithm
  constexpr zq_t operator*(const zq_t& rhs) const
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

    const uint32_t mask = (-static_cast<uint32_t>(t6 >= Q));
    const uint32_t t7 = t6 - (mask & Q);

    return zq_t(t7);
  }

  // Raises field element to N -th power, using exponentiation by repeated
  // squaring rule
  //
  // Taken from
  // https://github.com/itzmeanjan/kyber/blob/3cd41a5/include/ff.hpp#L224-L246
  constexpr zq_t operator^(const size_t n) const
  {
    zq_t base = *this;

    const zq_t br[]{ zq_t(1), base };
    zq_t res = br[n & 0b1ul];

    const size_t zeros = std::countl_zero(n);
    const size_t till = 64ul - zeros;

    for (size_t i = 1; i < till; i++) {
      base = base * base;

      const zq_t br[]{ zq_t(1), base };
      res = res * br[(n >> i) & 0b1ul];
    }

    return res;
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
  constexpr zq_t inv() const { return *this ^ static_cast<size_t>(Q - 2); }

  // Division over prime field Z_q | q = 2^23 - 2^13 + 1
  constexpr zq_t operator/(const zq_t& rhs) const
  {
    return (*this) * rhs.inv();
  }

  // Equality check between two field elements ∈ Z_q | q = 2^23 - 2^13 + 1
  constexpr bool operator==(const zq_t& rhs) const
  {
    return !static_cast<bool>(this->v ^ rhs.v);
  }

  // Non-equality check between two field elements ∈ Z_q | q = 2^23 - 2^13 + 1
  constexpr bool operator!=(const zq_t& rhs) const { return !(*this == rhs); }

  // Greater than operator applied to elements ∈ Z_q | q = 2^23 - 2^13 + 1
  constexpr bool operator>(const zq_t& rhs) const { return this->v > rhs.v; }

  // Greater than equal operator applied to elements ∈ Z_q | q = 2^23 - 2^13 + 1
  constexpr bool operator>=(const zq_t& rhs) const { return this->v >= rhs.v; }

  // Lesser than operator applied to elements ∈ Z_q | q = 2^23 - 2^13 + 1
  constexpr bool operator<(const zq_t& rhs) const { return this->v < rhs.v; }

  // Lesser than equal operator applied to elements ∈ Z_q | q = 2^23 - 2^13 + 1
  constexpr bool operator<=(const zq_t& rhs) const { return this->v <= rhs.v; }

  // Shifts operand ∈ Z_q, leftwards by l bit positions | q = 2^23 - 2^13 + 1
  constexpr zq_t operator<<(const size_t l) const { return zq_t(this->v << l); }

  // Generate a random field element ∈ Z_q | q = 2^23 - 2^13 + 1
  static inline zq_t random(prng::prng_t& prng)
  {
    uint32_t res = 0;
    prng.read(reinterpret_cast<uint8_t*>(&res), sizeof(res));

    // Modulo reduce random sampled 32 -bit unsigned integer value, because
    // explicit constructor of Zq expects its input ∈ [0, Q).
    return zq_t(mod_reduce(res));
  }
};

}
