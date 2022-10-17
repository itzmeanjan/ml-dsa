#include "bench_dilithium.hpp"

// register for benchmarking prime field arithmetic
BENCHMARK(bench_dilithium::ff_add);
BENCHMARK(bench_dilithium::ff_sub);
BENCHMARK(bench_dilithium::ff_neg);
BENCHMARK(bench_dilithium::ff_mul);

// benchmark runner main routine
BENCHMARK_MAIN();
