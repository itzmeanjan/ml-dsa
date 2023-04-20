## Benchmarking Dilithium PQ DSA

Benchmarking of Dilithium Post Quantum Digital Signature Algorithm can be done, by issuing

```bash
make benchmark
```

> **Note** Benchmarking expects presence of `google-benchmark` library in global namespace ( so that it can be found by the compiler ).

> **Warning** CPUs generally have frequency boosting technology, which should be disabled during benchmarking. You may use [this](https://github.com/google/benchmark/blob/60b16f1/docs/user_guide.md#disabling-cpu-frequency-scaling) as your guide.

> **Note** Only deterministic signing procedure is benchmarked, while signing message of length 32 -bytes. One can benchmark non-deterministic signing procedure by explicitly passing truth value to template parameter of `sign(...)` routine.

### On 12th Gen Intel(R) Core(TM) i7-1260P **[ Compiled with GCC ]**

```bash
2023-04-20T12:56:15+05:30
Running ./bench/a.out
Run on (16 X 4372.17 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 1280 KiB (x8)
  L3 Unified 18432 KiB (x1)
Load Average: 0.25, 0.40, 0.55
--------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations items_per_second
--------------------------------------------------------------------------------
dilithium2_keygen          72.8 us         72.8 us         9576        13.738k/s
dilithium2_sign/32          499 us          499 us         1788       2.00293k/s
dilithium2_verify/32       85.9 us         85.8 us         8159       11.6502k/s
dilithium3_keygen           119 us          119 us         5913       8.37128k/s
dilithium3_sign/32         1346 us         1345 us         2670         743.27/s
dilithium3_verify/32        136 us          136 us         5165       7.33807k/s
dilithium5_keygen           188 us          188 us         3760       5.31895k/s
dilithium5_sign/32          807 us          807 us         1000       1.23947k/s
dilithium5_verify/32        210 us          210 us         3335        4.7559k/s
```

### On 12th Gen Intel(R) Core(TM) i7-1260P **[ Compiled with Clang ]**

```bash
2023-04-20T13:01:45+05:30
Running ./bench/a.out
Run on (16 X 967.502 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 1280 KiB (x8)
  L3 Unified 18432 KiB (x1)
Load Average: 0.73, 0.77, 0.68
--------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations items_per_second
--------------------------------------------------------------------------------
dilithium2_keygen          63.3 us         63.3 us        11312       15.8016k/s
dilithium2_sign/32          979 us          978 us         2263        1022.09/s
dilithium2_verify/32       74.7 us         74.7 us         9361       13.3924k/s
dilithium3_keygen           106 us          106 us         6577       9.46013k/s
dilithium3_sign/32          569 us          569 us         1225       1.75808k/s
dilithium3_verify/32        118 us          118 us         5953       8.50925k/s
dilithium5_keygen           166 us          166 us         4189       6.02679k/s
dilithium5_sign/32         1955 us         1956 us         2191        511.343/s
dilithium5_verify/32        186 us          186 us         3752       5.37322k/s
```
