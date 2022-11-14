## Benchmarking Dilithium DSA

Benchmarking of Dilithium Digital Signature Algorithm, along with its components, can be done, by issuing

```bash
make benchmark
```

> **Note**

> Benchmarking expects presence of `google-benchmark` library in global namespace ( can be found by compiler ).

> **Warning**

> CPUs generally have frequency boosting technology, which should be disabled during benchmarking. You may use [this](https://github.com/google/benchmark/blob/60b16f1/docs/user_guide.md#disabling-cpu-frequency-scaling) as your guide.

### On Intel(R) Core(TM) i5-8279U CPU @ 2.40GHz **[ compiled with Clang ]**

```bash
2022-11-14T17:03:16+04:00
Running ./bench/a.out
Run on (8 X 2400 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB
  L1 Instruction 32 KiB
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB
Load Average: 2.00, 1.74, 1.77
----------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations
----------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                   3.75 ns         3.75 ns    186825094
bench_dilithium::ff_sub                                   3.74 ns         3.73 ns    188051730
bench_dilithium::ff_neg                                   1.24 ns         1.24 ns    552141916
bench_dilithium::ff_mul                                   8.25 ns         8.14 ns     84952487
bench_dilithium::ff_inv                                    127 ns          126 ns      8675069
bench_dilithium::ff_div                                    112 ns         99.6 ns      5421775
bench_dilithium::ff_exp                                    564 ns          561 ns      1243251
bench_dilithium::ntt                                      4500 ns         4436 ns       161580
bench_dilithium::intt                                     5031 ns         4894 ns       137676
bench_dilithium::power2round                              2.83 ns         2.60 ns    271443030
bench_dilithium::decompose<190464>                        8.85 ns         8.78 ns     81600298
bench_dilithium::make_hint<190464>                        16.9 ns         16.1 ns     48094760
bench_dilithium::use_hint<190464>                         8.59 ns         8.52 ns     77727687
bench_dilithium::decompose<523776>                        9.59 ns         9.52 ns     70827979
bench_dilithium::make_hint<523776>                        15.7 ns         15.6 ns     43788041
bench_dilithium::use_hint<523776>                         9.04 ns         8.96 ns     79465989
bench_dilithium::expand_a<4, 4>                          53887 ns        53593 ns        13103
bench_dilithium::expand_a<6, 5>                         107713 ns       106371 ns         7006
bench_dilithium::expand_a<8, 7>                         206827 ns       204590 ns         3182
bench_dilithium::expand_s<2, 4>                           7413 ns         7334 ns        83106
bench_dilithium::expand_s<2, 8>                          16387 ns        16203 ns        41158
bench_dilithium::expand_s<4, 6>                          18416 ns        18179 ns        38302
bench_dilithium::expand_mask<131072, 4>                   9344 ns         9003 ns        81601
bench_dilithium::expand_mask<524288, 5>                  10323 ns        10282 ns        63583
bench_dilithium::expand_mask<524288, 7>                  14912 ns        14782 ns        48256
bench_dilithium::sample_in_ball<39>                        703 ns          697 ns       969314
bench_dilithium::sample_in_ball<49>                        732 ns          728 ns       932736
bench_dilithium::sample_in_ball<60>                        960 ns          826 ns       835851
bench_dilithium::encode<3>                                87.4 ns         87.0 ns      7868882
bench_dilithium::decode<3>                                84.9 ns         84.6 ns      8065538
bench_dilithium::encode<4>                                36.8 ns         36.4 ns     20192347
bench_dilithium::decode<4>                                32.9 ns         32.5 ns     22325700
bench_dilithium::encode<6>                                88.5 ns         87.5 ns      8318380
bench_dilithium::decode<6>                                65.3 ns         64.6 ns     11060199
bench_dilithium::encode<10>                                152 ns          150 ns      4747372
bench_dilithium::decode<10>                                121 ns          117 ns      6058875
bench_dilithium::encode<13>                                142 ns          141 ns      4530862
bench_dilithium::decode<13>                                158 ns          157 ns      4703511
bench_dilithium::encode<18>                                195 ns          193 ns      3591014
bench_dilithium::decode<18>                                187 ns          186 ns      3698323
bench_dilithium::encode<20>                                282 ns          279 ns      2539627
bench_dilithium::decode<20>                                145 ns          144 ns      4831418
bench_dilithium::encode_hint_bits<4, 80>                  1459 ns         1454 ns       448643
bench_dilithium::decode_hint_bits<4, 80>                   111 ns          111 ns      6193868
bench_dilithium::encode_hint_bits<6, 55>                  2557 ns         2555 ns       277592
bench_dilithium::decode_hint_bits<6, 55>                   113 ns          113 ns      6190800
bench_dilithium::encode_hint_bits<8, 75>                  3316 ns         3314 ns       207207
bench_dilithium::decode_hint_bits<8, 75>                   156 ns          154 ns      4752626
keygen<4, 4, 13, 2>/32                                  121845 ns       121781 ns         5728 items_per_second=8.21146k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32         587904 ns       583484 ns         1000 items_per_second=1.71384k/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32       147741 ns       146429 ns         4598 items_per_second=6.82926k/s
keygen<6, 5, 13, 4>/32                                  222230 ns       219871 ns         3147 items_per_second=4.54812k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32       424601 ns       420275 ns         1697 items_per_second=2.3794k/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32     238140 ns       235943 ns         2863 items_per_second=4.23832k/s
keygen<8, 7, 13, 2>/32                                  335668 ns       333842 ns         2079 items_per_second=2.99543k/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32      3649194 ns      3615825 ns         1086 items_per_second=276.562/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32     398601 ns       389088 ns         1849 items_per_second=2.57011k/s
```

### On ARM Cortex-A72 **[ compiled with GCC ]**

```bash
2022-11-14T13:07:44+00:00
Running ./bench/a.out
Run on (16 X 166.66 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x16)
  L1 Instruction 48 KiB (x16)
  L2 Unified 2048 KiB (x4)
Load Average: 0.22, 0.05, 0.02
----------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations
----------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                   6.09 ns         6.09 ns    114868899
bench_dilithium::ff_sub                                   6.09 ns         6.09 ns    114913319
bench_dilithium::ff_neg                                   1.75 ns         1.75 ns    399354369
bench_dilithium::ff_mul                                   16.1 ns         16.1 ns     43480149
bench_dilithium::ff_inv                                   64.6 ns         64.6 ns      9992191
bench_dilithium::ff_div                                   78.5 ns         78.5 ns     10378972
bench_dilithium::ff_exp                                   1381 ns         1381 ns       507008
bench_dilithium::ntt                                     18307 ns        18307 ns        38212
bench_dilithium::intt                                    19235 ns        19235 ns        36401
bench_dilithium::power2round                              8.66 ns         8.66 ns     80813152
bench_dilithium::decompose<190464>                        32.7 ns         32.7 ns     21387354
bench_dilithium::make_hint<190464>                        46.3 ns         46.3 ns     15106160
bench_dilithium::use_hint<190464>                         32.1 ns         32.1 ns     21945419
bench_dilithium::decompose<523776>                        32.6 ns         32.6 ns     21450988
bench_dilithium::make_hint<523776>                        48.3 ns         48.3 ns     14489683
bench_dilithium::use_hint<523776>                         32.2 ns         32.2 ns     21730244
bench_dilithium::expand_a<4, 4>                         184038 ns       184037 ns         3800
bench_dilithium::expand_a<6, 5>                         348249 ns       348240 ns         2010
bench_dilithium::expand_a<8, 7>                         644214 ns       644203 ns         1087
bench_dilithium::expand_s<2, 4>                          24410 ns        24410 ns        28685
bench_dilithium::expand_s<2, 8>                          45569 ns        45568 ns        13817
bench_dilithium::expand_s<4, 6>                          45904 ns        45903 ns        14972
bench_dilithium::expand_mask<131072, 4>                  40743 ns        40743 ns        17185
bench_dilithium::expand_mask<524288, 5>                  50538 ns        50538 ns        13852
bench_dilithium::expand_mask<524288, 7>                  63863 ns        63862 ns        10961
bench_dilithium::sample_in_ball<39>                       2437 ns         2437 ns       282404
bench_dilithium::sample_in_ball<49>                       2638 ns         2638 ns       265358
bench_dilithium::sample_in_ball<60>                       2781 ns         2781 ns       244167
bench_dilithium::encode<3>                                 150 ns          150 ns      4672221
bench_dilithium::decode<3>                                 223 ns          223 ns      3100041
bench_dilithium::encode<4>                                72.8 ns         72.8 ns      9596181
bench_dilithium::decode<4>                                 149 ns          149 ns      4659261
bench_dilithium::encode<6>                                90.1 ns         90.1 ns      7770814
bench_dilithium::decode<6>                                 169 ns          169 ns      4119740
bench_dilithium::encode<10>                                201 ns          201 ns      3482111
bench_dilithium::decode<10>                                251 ns          251 ns      2772496
bench_dilithium::encode<13>                                278 ns          278 ns      2520722
bench_dilithium::decode<13>                                344 ns          344 ns      2030920
bench_dilithium::encode<18>                                457 ns          457 ns      1532283
bench_dilithium::decode<18>                                534 ns          534 ns      1314006
bench_dilithium::encode<20>                                387 ns          387 ns      1809067
bench_dilithium::decode<20>                                366 ns          366 ns      1917052
bench_dilithium::encode_hint_bits<4, 80>                  5371 ns         5371 ns       131609
bench_dilithium::decode_hint_bits<4, 80>                   439 ns          439 ns      1567029
bench_dilithium::encode_hint_bits<6, 55>                  8709 ns         8709 ns        79907
bench_dilithium::decode_hint_bits<6, 55>                   437 ns          437 ns      1603650
bench_dilithium::encode_hint_bits<8, 75>                 11570 ns        11570 ns        60233
bench_dilithium::decode_hint_bits<8, 75>                   585 ns          585 ns      1183561
keygen<4, 4, 13, 2>/32                                  465155 ns       465138 ns         1494 items_per_second=2.1499k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32        2314952 ns      2314940 ns          302 items_per_second=431.977/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32       563932 ns       563916 ns         1242 items_per_second=1.77331k/s
keygen<6, 5, 13, 4>/32                                  780548 ns       780538 ns          895 items_per_second=1.28117k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32      5175661 ns      5175611 ns          439 items_per_second=193.214/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32     886134 ns       886118 ns          790 items_per_second=1.12852k/s
keygen<8, 7, 13, 2>/32                                 1241698 ns      1241668 ns          562 items_per_second=805.368/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32      7302904 ns      7302872 ns           71 items_per_second=136.932/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32    1399345 ns      1399318 ns          500 items_per_second=714.634/s
```

### On ARM Cortex-A72 **[ compiled with Clang ]**

```bash
2022-11-14T13:10:05+00:00
Running ./bench/a.out
Run on (16 X 166.66 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x16)
  L1 Instruction 48 KiB (x16)
  L2 Unified 2048 KiB (x4)
Load Average: 0.48, 0.25, 0.10
----------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations
----------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                   8.27 ns         8.27 ns     84659525
bench_dilithium::ff_sub                                   7.26 ns         7.26 ns     96379146
bench_dilithium::ff_neg                                   2.18 ns         2.18 ns    321742655
bench_dilithium::ff_mul                                   19.6 ns         19.6 ns     35748546
bench_dilithium::ff_inv                                   54.0 ns         53.9 ns      9574180
bench_dilithium::ff_div                                   63.5 ns         63.5 ns     11018320
bench_dilithium::ff_exp                                   1480 ns         1480 ns       496987
bench_dilithium::ntt                                     16432 ns        16431 ns        42561
bench_dilithium::intt                                    20451 ns        20450 ns        34226
bench_dilithium::power2round                              7.40 ns         7.40 ns     94630467
bench_dilithium::decompose<190464>                        26.5 ns         26.5 ns     26373924
bench_dilithium::make_hint<190464>                        40.9 ns         40.9 ns     17114488
bench_dilithium::use_hint<190464>                         23.9 ns         23.9 ns     29246724
bench_dilithium::decompose<523776>                        23.1 ns         23.1 ns     30353698
bench_dilithium::make_hint<523776>                        37.9 ns         37.9 ns     18496065
bench_dilithium::use_hint<523776>                         22.6 ns         22.6 ns     30937160
bench_dilithium::expand_a<4, 4>                         161218 ns       161213 ns         4343
bench_dilithium::expand_a<6, 5>                         301031 ns       301014 ns         2326
bench_dilithium::expand_a<8, 7>                         566786 ns       566767 ns         1235
bench_dilithium::expand_s<2, 4>                          16767 ns        16766 ns        35917
bench_dilithium::expand_s<2, 8>                          39357 ns        39356 ns        17816
bench_dilithium::expand_s<4, 6>                          37291 ns        37289 ns        19113
bench_dilithium::expand_mask<131072, 4>                  35115 ns        35113 ns        19936
bench_dilithium::expand_mask<524288, 5>                  46621 ns        46619 ns        15015
bench_dilithium::expand_mask<524288, 7>                  65251 ns        65249 ns        10728
bench_dilithium::sample_in_ball<39>                       2070 ns         2070 ns       344926
bench_dilithium::sample_in_ball<49>                       2178 ns         2178 ns       325510
bench_dilithium::sample_in_ball<60>                       2326 ns         2326 ns       290839
bench_dilithium::encode<3>                                 167 ns          167 ns      4189295
bench_dilithium::decode<3>                                 184 ns          184 ns      3739025
bench_dilithium::encode<4>                                73.5 ns         73.5 ns      9520278
bench_dilithium::decode<4>                                 150 ns          150 ns      4561991
bench_dilithium::encode<6>                                 109 ns          109 ns      6434511
bench_dilithium::decode<6>                                 167 ns          167 ns      4109377
bench_dilithium::encode<10>                                210 ns          210 ns      3333836
bench_dilithium::decode<10>                                517 ns          517 ns      1363622
bench_dilithium::encode<13>                                218 ns          218 ns      3204451
bench_dilithium::decode<13>                                290 ns          290 ns      2436343
bench_dilithium::encode<18>                                291 ns          291 ns      2408301
bench_dilithium::decode<18>                                392 ns          392 ns      1801201
bench_dilithium::encode<20>                                350 ns          350 ns      2000965
bench_dilithium::decode<20>                                879 ns          879 ns       793476
bench_dilithium::encode_hint_bits<4, 80>                  5281 ns         5281 ns       132720
bench_dilithium::decode_hint_bits<4, 80>                   363 ns          363 ns      1875047
bench_dilithium::encode_hint_bits<6, 55>                  8282 ns         8282 ns        83868
bench_dilithium::decode_hint_bits<6, 55>                   404 ns          404 ns      1723358
bench_dilithium::encode_hint_bits<8, 75>                 11191 ns        11190 ns        62696
bench_dilithium::decode_hint_bits<8, 75>                   556 ns          556 ns      1259466
keygen<4, 4, 13, 2>/32                                  441272 ns       441245 ns         1577 items_per_second=2.26631k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32        2285939 ns      2285859 ns          423 items_per_second=437.472/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32       518581 ns       518555 ns         1348 items_per_second=1.92844k/s
keygen<6, 5, 13, 4>/32                                  734577 ns       734522 ns          953 items_per_second=1.36143k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32      5953070 ns      5952620 ns          102 items_per_second=167.993/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32     818662 ns       818625 ns          855 items_per_second=1.22156k/s
keygen<8, 7, 13, 2>/32                                 1170672 ns      1170651 ns          601 items_per_second=854.226/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32      3486761 ns      3486549 ns          201 items_per_second=286.817/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32    1297074 ns      1296986 ns          540 items_per_second=771.019/s
```

### On ARM Neoverse-V1 **[ compiled with GCC ]**

```bash
2022-11-14T13:13:27+00:00
Running ./bench/a.out
Run on (64 X 2100 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB (x64)
  L1 Instruction 64 KiB (x64)
  L2 Unified 1024 KiB (x64)
  L3 Unified 32768 KiB (x1)
Load Average: 0.56, 0.19, 0.07
----------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations
----------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                   4.64 ns         4.64 ns    151018042
bench_dilithium::ff_sub                                   4.63 ns         4.63 ns    151286746
bench_dilithium::ff_neg                                  0.963 ns        0.963 ns    726877819
bench_dilithium::ff_mul                                   12.7 ns         12.7 ns     54927048
bench_dilithium::ff_inv                                   40.6 ns         40.6 ns     13663235
bench_dilithium::ff_div                                   57.0 ns         57.0 ns     21378456
bench_dilithium::ff_exp                                    917 ns          917 ns       762165
bench_dilithium::ntt                                      5743 ns         5743 ns       121819
bench_dilithium::intt                                     6420 ns         6420 ns       109030
bench_dilithium::power2round                              3.99 ns         3.99 ns    175376738
bench_dilithium::decompose<190464>                        14.2 ns         14.2 ns     49181198
bench_dilithium::make_hint<190464>                        18.7 ns         18.7 ns     37480752
bench_dilithium::use_hint<190464>                         14.2 ns         14.2 ns     49422457
bench_dilithium::decompose<523776>                        15.3 ns         15.3 ns     45821021
bench_dilithium::make_hint<523776>                        20.7 ns         20.7 ns     33763777
bench_dilithium::use_hint<523776>                         15.3 ns         15.3 ns     45831946
bench_dilithium::expand_a<4, 4>                          99725 ns        99723 ns         7016
bench_dilithium::expand_a<6, 5>                         183171 ns       183167 ns         3822
bench_dilithium::expand_a<8, 7>                         341287 ns       341278 ns         2051
bench_dilithium::expand_s<2, 4>                          11754 ns        11754 ns        63715
bench_dilithium::expand_s<2, 8>                          23334 ns        23333 ns        30010
bench_dilithium::expand_s<4, 6>                          22678 ns        22678 ns        30510
bench_dilithium::expand_mask<131072, 4>                  19525 ns        19524 ns        35855
bench_dilithium::expand_mask<524288, 5>                  24005 ns        24004 ns        29164
bench_dilithium::expand_mask<524288, 7>                  25922 ns        25922 ns        27000
bench_dilithium::sample_in_ball<39>                       1252 ns         1252 ns       559634
bench_dilithium::sample_in_ball<49>                       1298 ns         1298 ns       515068
bench_dilithium::sample_in_ball<60>                       1434 ns         1434 ns       495647
bench_dilithium::encode<3>                                72.8 ns         72.8 ns      9626897
bench_dilithium::decode<3>                                78.8 ns         78.8 ns      8941643
bench_dilithium::encode<4>                                21.2 ns         21.2 ns     31866763
bench_dilithium::decode<4>                                29.8 ns         29.8 ns     23530400
bench_dilithium::encode<6>                                30.7 ns         30.7 ns     23000948
bench_dilithium::decode<6>                                58.3 ns         58.3 ns     12173802
bench_dilithium::encode<10>                               78.3 ns         78.3 ns      8945981
bench_dilithium::decode<10>                               87.8 ns         87.8 ns      7929685
bench_dilithium::encode<13>                                100 ns          100 ns      6978566
bench_dilithium::decode<13>                                109 ns          109 ns      6398987
bench_dilithium::encode<18>                                237 ns          237 ns      2960809
bench_dilithium::decode<18>                                202 ns          202 ns      3469458
bench_dilithium::encode<20>                                173 ns          173 ns      4050186
bench_dilithium::decode<20>                                143 ns          143 ns      4883476
bench_dilithium::encode_hint_bits<4, 80>                  2792 ns         2792 ns       244028
bench_dilithium::decode_hint_bits<4, 80>                   104 ns          104 ns      6721584
bench_dilithium::encode_hint_bits<6, 55>                  4907 ns         4907 ns       143321
bench_dilithium::decode_hint_bits<6, 55>                  94.1 ns         94.1 ns      7465147
bench_dilithium::encode_hint_bits<8, 75>                  6448 ns         6448 ns       108693
bench_dilithium::decode_hint_bits<8, 75>                   126 ns          126 ns      5555143
keygen<4, 4, 13, 2>/32                                  197283 ns       197279 ns         3577 items_per_second=5.06895k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32         387533 ns       387527 ns         1807 items_per_second=2.58047k/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32       223252 ns       223246 ns         3134 items_per_second=4.47936k/s
keygen<6, 5, 13, 4>/32                                  336325 ns       336316 ns         2082 items_per_second=2.9734k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32      7122360 ns      7122173 ns          100 items_per_second=140.407/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32     364235 ns       364229 ns         1923 items_per_second=2.74553k/s
keygen<8, 7, 13, 2>/32                                  559828 ns       559819 ns         1245 items_per_second=1.78629k/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32       891493 ns       891478 ns          785 items_per_second=1.12173k/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32     593198 ns       593185 ns         1180 items_per_second=1.68582k/s
```

### On ARM Neoverse-V1 **[ compiled with Clang ]**

```bash
2022-11-14T13:22:06+00:00
Running ./bench/a.out
Run on (64 X 2100 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB (x64)
  L1 Instruction 64 KiB (x64)
  L2 Unified 1024 KiB (x64)
  L3 Unified 32768 KiB (x1)
Load Average: 0.58, 0.31, 0.19
----------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations
----------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                   5.78 ns         5.78 ns    121082556
bench_dilithium::ff_sub                                   5.78 ns         5.78 ns    121132099
bench_dilithium::ff_neg                                   2.31 ns         2.31 ns    302849888
bench_dilithium::ff_mul                                   10.8 ns         10.8 ns     64898348
bench_dilithium::ff_inv                                   46.4 ns         46.4 ns     13611676
bench_dilithium::ff_div                                   54.3 ns         54.3 ns     13872297
bench_dilithium::ff_exp                                    647 ns          647 ns      1064478
bench_dilithium::ntt                                      9867 ns         9867 ns        70950
bench_dilithium::intt                                    11652 ns        11652 ns        60078
bench_dilithium::power2round                              6.55 ns         6.55 ns    106885180
bench_dilithium::decompose<190464>                        17.7 ns         17.7 ns     39501304
bench_dilithium::make_hint<190464>                        32.7 ns         32.7 ns     21377814
bench_dilithium::use_hint<190464>                         17.7 ns         17.7 ns     39500682
bench_dilithium::decompose<523776>                        13.7 ns         13.7 ns     51187322
bench_dilithium::make_hint<523776>                        23.7 ns         23.7 ns     29536199
bench_dilithium::use_hint<523776>                         13.3 ns         13.3 ns     52666460
bench_dilithium::expand_a<4, 4>                          75975 ns        75974 ns         9215
bench_dilithium::expand_a<6, 5>                         141161 ns       141158 ns         4959
bench_dilithium::expand_a<8, 7>                         263633 ns       263629 ns         2655
bench_dilithium::expand_s<2, 4>                           9655 ns         9655 ns        72767
bench_dilithium::expand_s<2, 8>                          17079 ns        17078 ns        37779
bench_dilithium::expand_s<4, 6>                          18706 ns        18706 ns        37478
bench_dilithium::expand_mask<131072, 4>                  12831 ns        12831 ns        54532
bench_dilithium::expand_mask<524288, 5>                  17118 ns        17117 ns        40891
bench_dilithium::expand_mask<524288, 7>                  23921 ns        23920 ns        29268
bench_dilithium::sample_in_ball<39>                        754 ns          754 ns       957904
bench_dilithium::sample_in_ball<49>                        827 ns          827 ns       856812
bench_dilithium::sample_in_ball<60>                        894 ns          894 ns       752242
bench_dilithium::encode<3>                                53.9 ns         53.9 ns     12999944
bench_dilithium::decode<3>                                55.1 ns         55.1 ns     12847183
bench_dilithium::encode<4>                                17.1 ns         17.1 ns     41036155
bench_dilithium::decode<4>                                30.5 ns         30.5 ns     22955405
bench_dilithium::encode<6>                                53.2 ns         53.2 ns     13167340
bench_dilithium::decode<6>                                59.7 ns         59.7 ns     11726185
bench_dilithium::encode<10>                               69.0 ns         69.0 ns     10151577
bench_dilithium::decode<10>                                230 ns          230 ns      3046507
bench_dilithium::encode<13>                               95.6 ns         95.6 ns      7332787
bench_dilithium::decode<13>                                112 ns          112 ns      6254551
bench_dilithium::encode<18>                                121 ns          121 ns      5766060
bench_dilithium::decode<18>                                150 ns          150 ns      4685788
bench_dilithium::encode<20>                               70.3 ns         70.3 ns      9952064
bench_dilithium::decode<20>                                348 ns          348 ns      2013383
bench_dilithium::encode_hint_bits<4, 80>                  2908 ns         2908 ns       243412
bench_dilithium::decode_hint_bits<4, 80>                   107 ns          107 ns      6682534
bench_dilithium::encode_hint_bits<6, 55>                  5001 ns         5001 ns       141030
bench_dilithium::decode_hint_bits<6, 55>                  98.5 ns         98.5 ns      7093051
bench_dilithium::encode_hint_bits<8, 75>                  6611 ns         6611 ns       105318
bench_dilithium::decode_hint_bits<8, 75>                   135 ns          135 ns      5177216
keygen<4, 4, 13, 2>/32                                  217523 ns       217515 ns         3243 items_per_second=4.59739k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32        1269837 ns      1269802 ns          763 items_per_second=787.524/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32       266975 ns       266970 ns         2621 items_per_second=3.74573k/s
keygen<6, 5, 13, 4>/32                                  352827 ns       352821 ns         1986 items_per_second=2.8343k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32      3676049 ns      3675981 ns          862 items_per_second=272.036/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32     408697 ns       408685 ns         1713 items_per_second=2.44687k/s
keygen<8, 7, 13, 2>/32                                  553158 ns       553146 ns         1267 items_per_second=1.80784k/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32      2498019 ns      2497899 ns          280 items_per_second=400.336/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32     636508 ns       636497 ns         1100 items_per_second=1.5711k/s
```

### On Intel(R) Xeon(R) CPU E5-2686 v4 @ 2.30GHz **[ compiled with GCC ]**

```bash
2022-11-14T13:27:04+00:00
Running ./bench/a.out
Run on (4 X 2300.25 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 256 KiB (x2)
  L3 Unified 46080 KiB (x1)
Load Average: 0.12, 0.14, 0.06
----------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations
----------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                   5.20 ns         5.20 ns    134341790
bench_dilithium::ff_sub                                   5.28 ns         5.28 ns    132650725
bench_dilithium::ff_neg                                  0.743 ns        0.743 ns    941933196
bench_dilithium::ff_mul                                   12.0 ns         12.0 ns     58405209
bench_dilithium::ff_inv                                    154 ns          154 ns      5683552
bench_dilithium::ff_div                                    121 ns          121 ns      6727577
bench_dilithium::ff_exp                                    885 ns          885 ns       817895
bench_dilithium::ntt                                      8522 ns         8521 ns        82105
bench_dilithium::intt                                     9328 ns         9327 ns        75048
bench_dilithium::power2round                              4.56 ns         4.56 ns    153546078
bench_dilithium::decompose<190464>                        16.9 ns         16.9 ns     41341463
bench_dilithium::make_hint<190464>                        29.1 ns         29.1 ns     24067326
bench_dilithium::use_hint<190464>                         15.7 ns         15.7 ns     44472763
bench_dilithium::decompose<523776>                        17.4 ns         17.4 ns     40308237
bench_dilithium::make_hint<523776>                        30.3 ns         30.3 ns     23085114
bench_dilithium::use_hint<523776>                         16.6 ns         16.6 ns     42232884
bench_dilithium::expand_a<4, 4>                         153426 ns       153426 ns         4562
bench_dilithium::expand_a<6, 5>                         283264 ns       283257 ns         2474
bench_dilithium::expand_a<8, 7>                         527083 ns       527071 ns         1326
bench_dilithium::expand_s<2, 4>                          14431 ns        14430 ns        48631
bench_dilithium::expand_s<2, 8>                          30230 ns        30228 ns        25018
bench_dilithium::expand_s<4, 6>                          28295 ns        28292 ns        24764
bench_dilithium::expand_mask<131072, 4>                  22112 ns        22110 ns        31663
bench_dilithium::expand_mask<524288, 5>                  27432 ns        27429 ns        25519
bench_dilithium::expand_mask<524288, 7>                  49400 ns        49400 ns        14188
bench_dilithium::sample_in_ball<39>                       1279 ns         1279 ns       556122
bench_dilithium::sample_in_ball<49>                       1352 ns         1352 ns       513003
bench_dilithium::sample_in_ball<60>                       1466 ns         1466 ns       476511
bench_dilithium::encode<3>                                 115 ns          115 ns      6100597
bench_dilithium::decode<3>                                 274 ns          274 ns      2448055
bench_dilithium::encode<4>                                47.2 ns         47.2 ns     13772685
bench_dilithium::decode<4>                                69.3 ns         69.2 ns     10582365
bench_dilithium::encode<6>                                 101 ns          101 ns      6779778
bench_dilithium::decode<6>                                 140 ns          140 ns      5073003
bench_dilithium::encode<10>                                242 ns          242 ns      2861911
bench_dilithium::decode<10>                                245 ns          245 ns      2895809
bench_dilithium::encode<13>                                303 ns          303 ns      2335335
bench_dilithium::decode<13>                                284 ns          284 ns      2397393
bench_dilithium::encode<18>                                335 ns          335 ns      2072402
bench_dilithium::decode<18>                                306 ns          306 ns      2320086
bench_dilithium::encode<20>                                344 ns          344 ns      2036109
bench_dilithium::decode<20>                                314 ns          314 ns      2226640
bench_dilithium::encode_hint_bits<4, 80>                  7593 ns         7592 ns        92212
bench_dilithium::decode_hint_bits<4, 80>                   215 ns          215 ns      3089495
bench_dilithium::encode_hint_bits<6, 55>                 11387 ns        11386 ns        61461
bench_dilithium::decode_hint_bits<6, 55>                   238 ns          238 ns      2959555
bench_dilithium::encode_hint_bits<8, 75>                 15192 ns        15192 ns        46076
bench_dilithium::decode_hint_bits<8, 75>                   317 ns          317 ns      2216253
keygen<4, 4, 13, 2>/32                                  298392 ns       298378 ns         2364 items_per_second=3.35145k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32        2737408 ns      2737371 ns          330 items_per_second=365.314/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32       336728 ns       336726 ns         2080 items_per_second=2.96977k/s
keygen<6, 5, 13, 4>/32                                  505332 ns       505329 ns         1384 items_per_second=1.97891k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32      6934334 ns      6934073 ns          100 items_per_second=144.215/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32     546213 ns       546157 ns         1280 items_per_second=1.83097k/s
keygen<8, 7, 13, 2>/32                                  831827 ns       831823 ns          843 items_per_second=1.20218k/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32      6825315 ns      6824846 ns          270 items_per_second=146.523/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32     896803 ns       896777 ns          781 items_per_second=1.1151k/s
```

### On Intel(R) Xeon(R) CPU E5-2686 v4 @ 2.30GHz **[ compiled with Clang ]**

```bash
2022-11-14T13:28:52+00:00
Running ./bench/a.out
Run on (4 X 2300.25 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 256 KiB (x2)
  L3 Unified 46080 KiB (x1)
Load Average: 0.47, 0.29, 0.13
----------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations
----------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                   6.31 ns         6.31 ns    110842225
bench_dilithium::ff_sub                                   6.31 ns         6.31 ns    110944662
bench_dilithium::ff_neg                                  0.991 ns        0.990 ns    706769010
bench_dilithium::ff_mul                                   12.5 ns         12.5 ns     55836648
bench_dilithium::ff_inv                                    101 ns          101 ns      5016956
bench_dilithium::ff_div                                    162 ns          162 ns      5499864
bench_dilithium::ff_exp                                    745 ns          745 ns       919617
bench_dilithium::ntt                                      6879 ns         6879 ns       101469
bench_dilithium::intt                                     7502 ns         7501 ns        93368
bench_dilithium::power2round                              3.89 ns         3.89 ns    179968326
bench_dilithium::decompose<190464>                        13.2 ns         13.2 ns     52954203
bench_dilithium::make_hint<190464>                        22.7 ns         22.7 ns     30851250
bench_dilithium::use_hint<190464>                         11.4 ns         11.4 ns     61452844
bench_dilithium::decompose<523776>                        13.5 ns         13.5 ns     51729556
bench_dilithium::make_hint<523776>                        24.0 ns         24.0 ns     29202580
bench_dilithium::use_hint<523776>                         12.2 ns         12.2 ns     57633888
bench_dilithium::expand_a<4, 4>                          71005 ns        71000 ns         9841
bench_dilithium::expand_a<6, 5>                         133052 ns       133045 ns         5253
bench_dilithium::expand_a<8, 7>                         243759 ns       243746 ns         2842
bench_dilithium::expand_s<2, 4>                          11984 ns        11983 ns        61268
bench_dilithium::expand_s<2, 8>                          23763 ns        23761 ns        29572
bench_dilithium::expand_s<4, 6>                          23874 ns        23873 ns        29469
bench_dilithium::expand_mask<131072, 4>                  13470 ns        13470 ns        51979
bench_dilithium::expand_mask<524288, 5>                  15026 ns        15026 ns        46570
bench_dilithium::expand_mask<524288, 7>                  21156 ns        21155 ns        33097
bench_dilithium::sample_in_ball<39>                        910 ns          910 ns       776226
bench_dilithium::sample_in_ball<49>                        980 ns          980 ns       701626
bench_dilithium::sample_in_ball<60>                       1050 ns         1050 ns       652282
bench_dilithium::encode<3>                                 104 ns          104 ns      6751890
bench_dilithium::decode<3>                                 118 ns          118 ns      5518104
bench_dilithium::encode<4>                                59.4 ns         59.4 ns     11780421
bench_dilithium::decode<4>                                45.9 ns         45.9 ns     16642732
bench_dilithium::encode<6>                                97.4 ns         97.4 ns      7268153
bench_dilithium::decode<6>                                86.6 ns         86.6 ns      7815614
bench_dilithium::encode<10>                                134 ns          134 ns      5465711
bench_dilithium::decode<10>                                123 ns          123 ns      5533619
bench_dilithium::encode<13>                                193 ns          193 ns      3585181
bench_dilithium::decode<13>                                577 ns          577 ns      1177626
bench_dilithium::encode<18>                                228 ns          228 ns      3075187
bench_dilithium::decode<18>                                536 ns          536 ns      1311408
bench_dilithium::encode<20>                                238 ns          238 ns      2939669
bench_dilithium::decode<20>                                189 ns          189 ns      3693831
bench_dilithium::encode_hint_bits<4, 80>                  2638 ns         2638 ns       267880
bench_dilithium::decode_hint_bits<4, 80>                   152 ns          152 ns      4626950
bench_dilithium::encode_hint_bits<6, 55>                  4590 ns         4590 ns       155480
bench_dilithium::decode_hint_bits<6, 55>                   153 ns          153 ns      4710163
bench_dilithium::encode_hint_bits<8, 75>                  5909 ns         5909 ns       115778
bench_dilithium::decode_hint_bits<8, 75>                   206 ns          206 ns      3387050
keygen<4, 4, 13, 2>/32                                  179138 ns       179128 ns         3920 items_per_second=5.5826k/s
sign<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32         660177 ns       660144 ns          777 items_per_second=1.51482k/s
verify<4, 4, 13, 2, 131072, 95232, 39, 78, 80>/32       214894 ns       214877 ns         3258 items_per_second=4.65382k/s
keygen<6, 5, 13, 4>/32                                  304221 ns       304205 ns         2307 items_per_second=3.28726k/s
sign<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32       626887 ns       626852 ns         1117 items_per_second=1.59527k/s
verify<6, 5, 13, 4, 524288, 261888, 49, 196, 55>/32     341508 ns       341501 ns         2049 items_per_second=2.92825k/s
keygen<8, 7, 13, 2>/32                                  473475 ns       473466 ns         1482 items_per_second=2.11208k/s
sign<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32      5147208 ns      5146651 ns          100 items_per_second=194.301/s
verify<8, 7, 13, 2, 524288, 261888, 60, 120, 75>/32     530196 ns       530200 ns         1321 items_per_second=1.88608k/s
```
