#include "bench_dilithium.hpp"

// register for benchmarking prime field arithmetic
BENCHMARK(bench_dilithium::ff_add);
BENCHMARK(bench_dilithium::ff_sub);
BENCHMARK(bench_dilithium::ff_neg);

// benchmark runner main routine
BENCHMARK_MAIN();
