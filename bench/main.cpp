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

// benchmark runner main routine
BENCHMARK_MAIN();
