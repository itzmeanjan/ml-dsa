#include "bench_dilithium.hpp"

// register for benchmarking prime field arithmetic
BENCHMARK(bench_dilithium::ff_add);
BENCHMARK(bench_dilithium::ff_sub);
BENCHMARK(bench_dilithium::ff_neg);
BENCHMARK(bench_dilithium::ff_mul);
BENCHMARK(bench_dilithium::ff_inv);
BENCHMARK(bench_dilithium::ff_div);
BENCHMARK(bench_dilithium::ff_exp);

// register for benchmarking (i)NTT
BENCHMARK(bench_dilithium::ntt);
BENCHMARK(bench_dilithium::intt);

// register for benchmarking coefficient reduction techniques
BENCHMARK(bench_dilithium::power2round);

// register for benchmarking sampling vector/ matrix from XOF
BENCHMARK(bench_dilithium::expand_a<4, 4>);
BENCHMARK(bench_dilithium::expand_a<6, 5>);
BENCHMARK(bench_dilithium::expand_a<8, 7>);
BENCHMARK(bench_dilithium::uniform_sampling_eta<2, 4>);
BENCHMARK(bench_dilithium::uniform_sampling_eta<2, 8>);
BENCHMARK(bench_dilithium::uniform_sampling_eta<4, 6>);

// register for benchmarking serialization/ deserialization of polynomials
BENCHMARK(bench_dilithium::encode<3>);
BENCHMARK(bench_dilithium::decode<3>);
BENCHMARK(bench_dilithium::encode<4>);
BENCHMARK(bench_dilithium::decode<4>);
BENCHMARK(bench_dilithium::encode<10>);
BENCHMARK(bench_dilithium::decode<10>);
BENCHMARK(bench_dilithium::encode<13>);
BENCHMARK(bench_dilithium::decode<13>);

// register for benchmarking Dilithium Key Generation Algorithm
BENCHMARK(bench_dilithium::keygen<4, 4, 2, 13>); // NIST security level 2
BENCHMARK(bench_dilithium::keygen<6, 5, 4, 13>); // NIST security level 3
BENCHMARK(bench_dilithium::keygen<8, 7, 2, 13>); // NIST security level 5

// benchmark runner main routine
BENCHMARK_MAIN();
