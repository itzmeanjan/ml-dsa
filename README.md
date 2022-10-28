# dilithium
CRYSTALS-Dilithium: Post-Quantum Digital Signature Algorithm

## Motivation



## Prerequisites

- A C++ compiler with C++20 standard library such as `g++`/ `clang++`

```bash
$ clang++ --version
Ubuntu clang version 14.0.0-1ubuntu1
Target: aarch64-unknown-linux-gnu
Thread model: posix
InstalledDir: /usr/bin

$ g++ --version
g++ (Ubuntu 11.2.0-19ubuntu1) 11.2.0
```

- System development utilities such as `make`, `cmake` & `git`

```bash
$ make --version
GNU Make 4.3

$ cmake --version
cmake version 3.22.1

$ git --version
git version 2.34.1
```

- For benchmarking purposes, you must have `google-benchmark` library globally installed. I found [this](https://github.com/google/benchmark/tree/60b16f1#installation) guide to be useful.

- Note, `sha3` is a dependency of this project, which is pinned to a specific commit, using git submodule. Import `sha3` dependency after cloning Dilithium.

```bash
git clone https://github.com/itzmeanjan/dilithium.git
cd dilithium
git submodule update --init

# now you can {test, benchmark, use} `dilithium`
```

## Testing

For testing functional correctness of Dilithium Digital Signature Algorithm, along with its components, issue

```bash
make

[test] Dilithium prime field arithmetic
[test] (i)NTT over degree-255 polynomial
[test] Extraction of high, low order bits using Power2Round
[test] Making and using of hint bits using Decompose
[test] Hashing to a ball
[test] Polynomial encoding/ decoding
[test] Hint bit polynomial encoding/ decoding
[test] Dilithium KeyGen -> Signing -> Verification
```

## Benchmarking

Benchmarking of Dilithium Digital Signature Algorithm, along with its components, can be done, by issuing

```bash
make benchmark
```

> **Note**

> Benchmarking expects presence of `google-benchmark` library in global namespace ( can be found by compiler ).

> **Warning**

> CPUs generally have frequency boosting technology, which should be disabled during benchmarking. You may use [this](https://github.com/google/benchmark/blob/60b16f1/docs/user_guide.md#disabling-cpu-frequency-scaling) as your guide.

### On Benchmarked on Intel(R) Core(TM) i5-8279U CPU @ 2.40GHz **[ compiled with Clang ]**

```bash
2022-10-28T16:02:03+04:00
Running ./bench/a.out
Run on (8 X 2400 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB
  L1 Instruction 32 KiB
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB
Load Average: 1.99, 1.83, 1.89
------------------------------------------------------------------------------------------------
Benchmark                                                      Time             CPU   Iterations
------------------------------------------------------------------------------------------------
bench_dilithium::ff_add                                     3.78 ns         3.77 ns    187104741
bench_dilithium::ff_sub                                     3.72 ns         3.72 ns    187291972
bench_dilithium::ff_neg                                     1.34 ns         1.33 ns    554635565
bench_dilithium::ff_mul                                     6.12 ns         6.10 ns     90852456
bench_dilithium::ff_inv                                      115 ns          114 ns      7266942
bench_dilithium::ff_div                                      107 ns          106 ns      5066736
bench_dilithium::ff_exp                                      359 ns          352 ns      2180753
bench_dilithium::ntt                                        4956 ns         4910 ns       138608
bench_dilithium::intt                                       4580 ns         4545 ns       154970
bench_dilithium::power2round                                2.44 ns         2.43 ns    279408771
bench_dilithium::decompose<190464>                          8.67 ns         8.63 ns     81950900
bench_dilithium::make_hint<190464>                          14.2 ns         14.1 ns     47804738
bench_dilithium::use_hint<190464>                           8.07 ns         8.06 ns     82381048
bench_dilithium::decompose<523776>                          8.82 ns         8.81 ns     78788015
bench_dilithium::make_hint<523776>                          14.6 ns         14.6 ns     47386627
bench_dilithium::use_hint<523776>                           8.40 ns         8.40 ns     82389774
bench_dilithium::expand_a<4, 4>                            53043 ns        52994 ns        13008
bench_dilithium::expand_a<6, 5>                            97913 ns        97816 ns         7114
bench_dilithium::expand_a<8, 7>                           186213 ns       185641 ns         3714
bench_dilithium::expand_s<2, 4>                             6910 ns         6905 ns        92842
bench_dilithium::expand_s<2, 8>                            15339 ns        15323 ns        49008
bench_dilithium::expand_s<4, 6>                            16730 ns        16718 ns        41643
bench_dilithium::expand_mask<1u << 17, 4>                  30340 ns        30324 ns        22842
bench_dilithium::expand_mask<1u << 19, 5>                  41164 ns        41138 ns        16949
bench_dilithium::expand_mask<1u << 19, 7>                  57781 ns        57744 ns        11784
bench_dilithium::sample_in_ball<39>                          611 ns          611 ns      1107823
bench_dilithium::sample_in_ball<49>                          668 ns          664 ns      1012482
bench_dilithium::sample_in_ball<60>                          767 ns          766 ns       846238
bench_dilithium::encode<3>                                   965 ns          964 ns       721560
bench_dilithium::decode<3>                                  1019 ns         1019 ns       672922
bench_dilithium::encode<4>                                   843 ns          842 ns       819499
bench_dilithium::decode<4>                                  1031 ns         1030 ns       674328
bench_dilithium::encode<6>                                  1714 ns         1711 ns       409333
bench_dilithium::decode<6>                                  1832 ns         1831 ns       379451
bench_dilithium::encode<10>                                 2904 ns         2902 ns       239837
bench_dilithium::decode<10>                                 3130 ns         3127 ns       224900
bench_dilithium::encode<13>                                 4306 ns         4303 ns       162517
bench_dilithium::decode<13>                                 4653 ns         4651 ns       150523
bench_dilithium::encode<18>                                 4994 ns         4991 ns       137373
bench_dilithium::decode<18>                                 5817 ns         5813 ns       118850
bench_dilithium::encode<20>                                 5530 ns         5525 ns       123767
bench_dilithium::decode<20>                                 6456 ns         6452 ns       107137
bench_dilithium::encode_hint_bits<4, 80>                    1572 ns         1571 ns       432764
bench_dilithium::decode_hint_bits<4, 80>                     102 ns          102 ns      6652664
bench_dilithium::encode_hint_bits<6, 55>                    2659 ns         2658 ns       266222
bench_dilithium::decode_hint_bits<6, 55>                     108 ns          108 ns      6501045
bench_dilithium::encode_hint_bits<8, 75>                    3557 ns         3554 ns       201718
bench_dilithium::decode_hint_bits<8, 75>                     143 ns          142 ns      4849661
keygen<4, 4, 13, 2>/32                                    156827 ns       156752 ns         4479 items_per_second=6.3795k/s
sign<4, 4, 13, 2, 1u << 17, 95232, 39, 78, 80>/32         514968 ns       514732 ns         2054 items_per_second=1.94276k/s
verify<4, 4, 13, 2, 1u << 17, 95232, 39, 78, 80>/32       184928 ns       184796 ns         3759 items_per_second=5.41138k/s
keygen<6, 5, 13, 4>/32                                    252423 ns       252278 ns         2777 items_per_second=3.96388k/s
sign<6, 5, 13, 4, 1u << 19, 261888, 49, 196, 55>/32       733957 ns       733467 ns         1422 items_per_second=1.36339k/s
verify<6, 5, 13, 4, 1u << 19, 261888, 49, 196, 55>/32     280147 ns       279838 ns         2491 items_per_second=3.57349k/s
keygen<8, 7, 13, 2>/32                                    388033 ns       387809 ns         1805 items_per_second=2.57859k/s
sign<8, 7, 13, 2, 1u << 19, 261888, 60, 120, 75>/32      1064153 ns      1063215 ns          959 items_per_second=940.544/s
verify<8, 7, 13, 2, 1u << 19, 261888, 60, 120, 75>/32     436424 ns       436093 ns         1604 items_per_second=2.29309k/s
```

## Usage

`dilithium` is a zero-dependency, header-only C++ library such that it's fairly easy-to-use

- Clone Dilithium repository
- Fetch only dependency `sha3`, by enabling git submodule

```bash
pushd dilithium
git submodule update --init
popd
```

- Write program which makes use of Dilithium {keygen, signing, verification} API, while importing `include/dilithium.hpp`.
- Finally compile your program, while letting your compiler know where it can find Dilithium and Sha3 headers.

These are the namespaces of interest

Namespace | What does it hold ?
:-- | --:
`dilithium::*` | Dilithium key generation, signing and verification algorithm implementation, which can be parameterized to act like either of Dilithium{2, 3, 5}
`dilithium_utils::*` | Several utility routines, among those worth mentioning are routines which can be used for compile-time computing how much memory to allocate for storing Dilithium public key, secret key & signature, for specific NIST security level parameters.

> **Note**

> Find Dilithium parameters in table 2 of [specification](https://pq-crystals.org/dilithium/data/dilithium-specification-round3-20210208.pdf).

**Dilithium DSA API Usage Flow**

- (i) Generate key pair using `dilithium::keygen<k, l, d, η>(...)`
- (ii) Sign N (>0) -bytes message using `dilithium::sign<k, l, d, η, γ1, γ2, τ, β, ω>(...)`
- (iii) Verify signature using `dilithium::verify<k, l, d, γ1, γ2, τ, β, ω>(...)`

What does it do ? | Header | Namespace | API | Inputs | Outputs
--- | --- | --- | --- | --- | ---
Key Generation | [keygen.hpp](./include/keygen.hpp) | `dilithium::` | `keygen<k, l, d, η>(seed, pubkey, seckey)` | 32 -bytes seed | Public key and secret key bytes
Signing | [signing.hpp](./include/signing.hpp) | `dilithium::` | `sign<k, l, d, η, γ1, γ2, τ, β, ω>(seckey, msg, mlen, sig)` | Secret key, N -bytes message s.t. N > 0 | Signature bytes
Verification | [verification.hpp](./include/verification.hpp) | `dilithium::` | `verify<k, l, d, γ1, γ2, τ, β, ω>(pubkey, msg, mlen, sig)` | Public key, N -bytes message s.t. N > 0 and Signature bytes | Boolean value denoting success during verification

I suggest you look at following three example programs, which demonstrates how to use Dilithium DSA API

- For Dilithium2 ( i.e. parameters providing NIST security level 2 ) see [dilithium2.cpp](./example/dilithium2.cpp)
- For Dilithium3 ( i.e. parameters providing NIST security level 3 ) see [dilithium3.cpp](./example/dilithium3.cpp)
- For Dilithium5 ( i.e. parameters providing NIST security level 5 ) see [dilithium5.cpp](./example/dilithium5.cpp)

```bash
$ g++ -std=c++20 -O3 -I ./include -I ./sha3/include example/dilithium2.cpp && ./a.out
Dilithium @ NIST security level 2
seed      : 66431d9b73cabf480808dc7a367f9cbc2772e91002027216c5b6f64905cdb008
pubkey    : 5e45fc0f989da464c78c3902c73f5d24ab4d5fc7dc12c4e100499e1a7cad96df7a44cba5c3d9f7dd40cdeeb88d549e9075653b7a27c86df19004d20acf9001cdc4d78036758aa38b64316d43432b7a9ed99350f5bc10b382d402625d666c0b50243f479da1d1f5d08d75ce226a3477e06a130b0cf5bd2026ebfe55e1e7bc83108f4e94e3ff90f6f25d7fe62fb0547b86c3dfd76c04da17323358b8fc61efe8e3caaacde0ebd862d7af3f580c18c43526c48d7a38903c271d4693a48408a242b01cda34e08f20750e12713e2db25855a1d7d725f702f853eb20175dfec28aa5e010204bcb0d4d544e5a7b5bf4f0ad1da8630a6f664dfdf3e0c64ac5da8ea242214186707ecc775959b9c384dab883df415ad10466c68de85856609f551dec408599c8f84d0d39fbf73667a22d5e82efce9374e09c9f32615f81fe4c9eb7c9bc5a6725a86006cfef121e4a9e5f152d0927bbe436856500fa215e313645661f307d7de4f0a0d712792ecba39bd11c444251115dc9e15bba129e92289b27c57561a34524efbd23568b99cf1a08d9ea0d48fd0796bfc7bcb87c909cd19c726633da879559187e9d5f71ca3e141872bd94a197f2301da444088b6caaa20134254d48e406de92e88a9ea35c1d7be8fff318bed1068ce3e4120f8041cdc3c0881b771b453d6c80f257840c476d1d367245c37649f7c7e38eb6896c77c1316e9fe1c0c876c5bac7be7873ea80d885f2b9b8eb5ed7426726974a3e7d91ae8f4acba4ebe723db638dee7a47d08cf446373fcb934cdfa10b77d7c2d01b526c58b6dbccfffad683e9207b56684d1fafd2e6352a56c94abd298daa13c7c2d08e1717b70663a85eefc12dfae934a1098e471def6507d671bd706c8ea86a599ec8b9aeb2aac2785853a5b934665113d1d4553cc1971a2b71f206fba6ba68fc63669a5b82dbb7ba64885ebb81134d7834056825da407f81fc5cde5c08c41ca5c5f7a2094a34949d8d545f115412fd7d702479cdd06eca97122515be7ffd98cae36ec482d1e16e1bd4e926e4f5b5f68b695aac5749694f401bd388e5e82112d0c07654190d510915cfc0394e43fb676a24ca0b4d4a902392c82537e72963b25b68d70e31922839374b7aabba5eff13f391b16a26481bcb2813766c14f96fec2eb0130cae84a0858dac9debe27bc2ebdfd39f942730d0b6674f2e2460f2881a4233c790ba084a409c63c437b06d306c3366b18b2aa397aa7f6ce0d9f7188f2fa23a52032469a9b8a6e6a1a05c3b570126f999c97787a42126bf324715e749a5021536552c5604fbaa74f862f5f77a0b9582548e994965c8d6c12bd4629039d6443ad166e7b27aa4d4ab2a089b8a997ab813e7a0fe2bccf1b0bccca9dc043941f5917fcd6689e7ec59644d1013392a788549d458956b497afaa24fe936bb478037e6d9b573edbae80660b922d331cb06ba8570df32ae222726a6a9ef14dc0a4c7f5707d99cfbed10cdf604c3ab5d695ee7b0b675e8c9453aff75563490e3e92acc51dd692d29da6945661d398825f07761b35cbd2d90c7f430e1d899f274de73df5569b7abd54009e16c2b062aca866dd78cc2b44a2c387e4210e6259f187445889a98c1167db49018c606106db824eaad0a328096c7d0c018dcb569a8908c7b88600d49c74db878c054f61f1e9cfb532fb8fe0714c96056e03bb4fb7f2f4de98b0c9e4d6f33a2ec8b60d308e010a52cfa5c6af1f05322ac0ec259d6cd322ad3aad553265491e313f867ed0bf5ab2f879c6e64208c404320622a2e562454e4ee26ba27a47ecd3ccba38355ba9094722b03a9222a454334c124bdd35186f26ad6415f7be795603f9f4e07
seckey    : 5e45fc0f989da464c78c3902c73f5d24ab4d5fc7dc12c4e100499e1a7cad96dfa29f9939512851d70089a4526d82a18acdf1873783221406f36e3a4cfa08812d09f3f0d50c5e2b5472b3549d2de5e5ddb3d092cfe8185ff8bc4a5e710616def60ab48148b80d1ac248d2468ca11005e4320613102512c5885126911b234c01826141128d49180e00a16c632492800262944428c1b6311a958c13926cc2228641866c5132250443860225451a2809410809a114121b02821c200841c80001c30044b82d59c610903672c0904d9188200144041a93045bc66444a4314bb669603080238100991802014564c8162263060d61204842224cc408904ab270da120844b685d442045208311283898b46684ac22ddb9221a324694a188601110c5c468181842940802501805008352e93a40d0396712227441336421b15459cc009934692242700c1866c9c9411930242c84272231420a2c82d0b463189283099468113c76542464ca1228ad318260c286dd4324084868982b42181a82c11322a4900425040055ab2604c2650423866e3226113312c423266598284db306a121951c2800508c591c3844d193380a3a8601b0226c3282e04c008db2084024088083489101341c0384062200d62b820d0b285c910611a360c5ac031481849441431e4a28409940851a28453a421e1200281c0605a1270523641a480809b148ca3265052b20d4422462419481a386e5b426c1b152602c090c10201843429e02205cba4288aa2298ba48121b068092690e0960d53906d5a84255402061b910c0b01699b1480013265d4364e44206564265061004c0b444d00384501a66c0b829004a51124480222490121b04913092151265021914411c96c0a36720b2120d24631d2400a1041800c99291894088a300e19912561448e2181091bb3305486400ab5411c38869cb44c92148e03486c43b260cc260080c25111a80191266842b04124b78904a304223065e3008c823864cba44d104045a1c86c94224262c07011b068cc207020c02961484d8b008a9180840b972013310a9a10910bb4685c3842182369c81028cba665c994111a072e1497451997450222654184481326490a152d88184c49c009d0929110312ca49069044310228550844642c124109cc40d11a685e0220600322620088e49324ca3b264c9b641c348689332044896848b4432a4a8440b4949d18ad62b5e1592b3f232ee43fc96d88e5606be9e9a7fe42aa360d319521114b3d17b480393bf68ced1135ec6c01b05a154a554dbdad8f4a6379e8bd92beca8bc7fb9053e0c49aeaee6810d10fb5ac990301275ccd33114a54594f8993bd6076f2a66e68a78bd662a90c9eaf3cfd286c7486a89937cddb4f823c949e8ed66975cadf6001393a62dec3622f6369ec2365585b744cb53058cb819b9362647529af98c84cea2f86326b07a2444f722f518848cc3672a44c5c1c6f4ee89a411a54ffd5a71b3dde5abf3e5eb47cb5d14b921a9d538f97722fbc8183de2c8b9f230eabd06d5c51469d234854bce7dfee9fb5ef61d41ab689e29b103ec102e6f1358e0dc17b0b129cebd1e298c21b6412fa52c5843d517edcf156224a21174865723b6e34a84167ff0a43b8e063b54cb3a0c3eac7e524f41545b98c7f36ce8e3567fc9bc5f9319168ebb133021ff07e091bcdb525343597725df4eaaf7a4386618e74028b07bd1574d0bd5f31df2a00dc3d656ade0663b06032d0cf66ce213f84110804e92b6c17c049edb975cdb28052e7348a916caa4a90045b3be872b2403299b2f1b5710235ff7742cebc54d5af85c10ddb8b3bf06075b6dd1f94961db13f13034a7dec39d1e8e875b198be596c2df7f6fd611a46aae8e622dec963055700a396fa25e3a87de9f67dfa532dc81c53bde853d25defb9bc437e3a8d576f1f1b883a8c5f2de6286aef308f8a159f724e4f72a9afeafd8ea638dae797c72d75e890d48546c9c4a98d157c1a2df5729d0e83b5ba2a363bc9c17ba7a4838bafb8a316c34788c10f21193d5c04595d5202ebc94fe477ac1aad036748ae25a0e21ac2d0b733be60f59094823cb9ace4023e0925e5834e8d06a366bbced957e60059a56a2bff4cf2704e5fa0cbd89c7ab4b2b30f82e59162acf2527fc9fbf264e01fe2bcd59e323760984e044ccda664f3f54cdbf40c204b693e1bf2476104abb91e0e0faf860e422f55ccb62e3db6533e3cf5ca1434f1e7d53ba394bcf22206549925fcebc1951295eb00a4370a8f2b3edcacacf586a189d7e42554168db6124e1fa59206169d52700cddf049e72d2a0c3c2cd634ec10083147a88bc29259717d652463108369d800cc4e39d3d0a4e63aa957d785370453fdba05d8efa41f015a0cd785ff60c62e7c9c84a76fade9217bc85c59a6de8eef69bf22ce16e5e9501b1e65d37aed2c18758d44c5f8c58bd9a997cf891d9500d82fbe53adfc91a37391b67bdcdcb1b0152005362117bc444245c6cdd2dc091e7334a3f366220ac44f900d919ec9a1509104dc6c8ac2a84ee05f41d244943432564bdc3ea6a1ab49c4a7109425aee30230a7b9cb5654de792e31604b8fedc535747db6732574102ce448dabca684691a40c0028b17b973e82060e390a13f2fc6c24243f8371eb8170151c06e8e2f03476b2b73f2efa87d34cb6da09362c8bf9ea116f2ab1cf54f9a41a43e3bbd511df8f5c8148f3b538ed90c70e9c919572025a7022380b6b7825614f6ab050422d3a376e6d56470d755af32477eb18dbe38860743d5622f3ba988ad54fb55db70073a429c84db3c060b5f7143b489eec927814dd0ed8f8c379da918239f1d4fdcbac6e65486084484320f60f1c51e67476e875f8549bba3a137ff6dc9a02fb59742323dbde614819a47e05d5a83a45854f9affb393f6d4bcab978ee6ffb8df1a9a3a0d8e531c9dd94df299121647b1d14da7c0a523d638224f41d7bd1c12445f6699a6c28768f9f6e31284987908bb1030817856158ea4f9c9c259e867187a02c0c375d3479a7bfaf4e97f59d53eb567a402c09adf8e80934e9a7c917b1e25b50222d269ba4d2b4606266ebd16b29864e1785b8866394759f0a9feabf51be43d98fa457ddca77fa3046f1420e00b8a26787c15948792004b113f64e3bf22b386be2b87338922f6f277eb392d68318598555823315ddde630b0f33c066730075a8e4dfe91de57d2eeee754e390fa24812ff59a7f51a79ddb79cf947e3f23504903447c59aa31ae858afc2d2c28e9152f319af006e249cfa1912bb90f14b74779a71b249b07c2fc4bfc3377dcc566b5508ab21d28577384b14891c34f99b13022b8ad9455d2e7f02bcb7d218ece2b228f076818c710b757f0b30226d47a05e3a1bec0535449449ce06c5172c04060f89a4ebec6748190fc47935842aada7de3293a52a0ef4ae136d79e8f22871a050c3ab6d5233ad6f914becbf58334e13ec68ae78d02959106a0b7c02b6d52b5889ec07fdd188babbb5b3d7253bb8721322e3fab09dc1ee8e5c8675180caf202d4a8b6571acc71952934ffd3676fac8b32043a1346b
message   : 7f244373d8af9b9c12ac5ed139804872d5b86d85eae11dd23a66ecbf2d396725
signature : 97a3eaff2cce526223227d857dfc7e9d47213cad12d21cb326c1c55c250a5cd10c9c5a9c391ebb491142ab93e362f9b33667e744d3c1f58e42e212bc5673df632a52d60459310a7d8602998a6e34cdd80ad8ed2c67c5c339460adbe0aeaf70830f62b629cfb0dda60355f51851f4d2e588e00f6c37f08eeba5bc028dc63643b742c6af24e9c981d7fad0b856f77b1f93d45d944da3fe7544015c868c735bc30ca39145b8eb7b370c807e9d4a55c5f203db6d51be6c395f0c3611023c8a8a8a8b4d6b72b360ff087c7b89da4e41b9447a72ef6c967e4a80f70ff830714359f24e79d8a5121069ad56b0823bab2afcb745373c91a7372ac5ffbca6f3cdb5b181c5e0b5fbbe6abe1052aa7980f5f7009d8f7a0291b741cd08cd4b65f980bd99ed5ef4a35be28ebe1f833e239845265636542335da3f8d1ffacdf7b18119495f0e06d82188f523ca2b9c1c20b424cea293c4b2b63f142a7973972682229f86713ef99ae305084d6e72479e99ba7251e7f6d5795f2a1f7ba87c56f5f6661260e17b8769ada07d6cfaca1d36733f96d91849c4e5fa7732a43aba7eecdd34e6a429dfd2cffc9a31408fbba6d9baee7e10462b01343d0824ccaabe5f32ba128af2c754b487006adac4121371573eb039f21b5c1fd7010024cb2ab6422dfaec86ef641cfe24e14226074ea454215182bc240ae970ae3aa0eb0e7ae34451755e4e0e2daa436c616ab3619edcdb324967806706368565176a01c8683901edd5a2f4f6cb3e36d21a928e82092badbbdeaf1f4ffc1174c1f164c049febd179f0dad75cd6288dfbadb2cec2e5dd151a59d8c6c9ba12522c1e969b821ae45b46dd4f1f6e90d77d0bdabee2707d5485d25c5acd6fa17ee996001dfe7c69b723078fd4d1a543ab4e586d9d98c11c1d6b4f2ba983b9506da6f60ed9e146514fe64f4b7759016d925f340321949632ba47a87066e42f89d1bcf26da930f42eb367f0fcadafb64da503c5502a07f11d17504ba9d92432f8b145a6f942f95f957f50701e94405295af59930167ff75270b9b0e5a255d22972e6fcc0f597c79795e14ec997b32a27e6edcca6adafbc755f72da9f5c5c019a2828eee6ea2735cb6c85a729cd61c123d4925d6ba51fe897aaff696f2b8e574e42947900246e4fd13de44d6e4b384780f2eed0626c30803d9e678969baf9fce2ea946c54017e6b9ec241d03ba5eacab0c45909ddd7cda542f7fca7362568fca258b465dea34644c464ac97a680e53ea5d3f375cf19227a6c19950d0d745c6c923be7374c37391ece778ecdc9631abfd9e589076ca4fef74074ef3984c8777b152117817a570f7ab1d17e28980f3d66434e7cff3f7369055875d87a04f4955648834d8bd71d01c2ce202f903f2d6e7ffedbad3faf7602ad80630d6fc9089615ef076405f2f457b6696635a51d5936ea823776da5cfca1bf82e565e8647e315adec048fb79916f348d8d9ec67e9f0273ca10f9bc4af915ea6db45ed3cd62874fc4753bdd3f87e2617f8d15373720a679b6aaabd64e5e2ba2606a0c4b95998fc63e91f25f5cd48628dbe1e94161e4eb65866f297fca3eee52c78ff1c259a46174e9268930f53da3755a0729bba229efe875db9e19818870178126fe2df88f674c356459877df501a71a115b53bf4ab74eafae0b7310c82934e00b4503b0ee6693fa078fa4ed0859b72b8fecb33095195ef7919ba634af92c26cde202080c314c351f6e6bcf2759a69b3488ec9af93012fc544e8ec46a9dd0de4de71c2f1e0d4dc5e6d0f67e188f27bc91b3c480b3c25cc7b9ef532da201a3294e34062e5b351b482df1fb1531e1b781a45a96e7af12c590e90663fff82b220523d2baf6480a2ddeed85de47cbe6468d6a97a3a17425c6392d7509be05797f0c85c3a7d854ae235a2e069ab6aa8e35b9d9322828ba448aa0527e675451eb3659cb57f32d08397c07967e7e486c9b852d69753e7372b230ae464c26f0337bc001351e469d10076f1a10fee8cdfd71c49767f22b9abf64c7c0e15931a4a7676e6559aa8c4337c06bb4c859ebadd033fa9cbbb9d80908b1f2c1fc4fc745a5ee6bc3506550e65bb122b0eac976f85a24d369c5b739d03e1eb5a45987e5364407fccd42680b3d58da1c8286f0380e3e5f9a81a569fcf8a68c43d8755e378b85721c85d07f2ed3e55375faeeaeaf457198b386f5d7ccb9e27fee3cc59ff7b9ce2c480f27e7ffb8b1f80b779d741251b55dd9e335b9c5a077d3dab73519f2aa6983073070cba224e54d55fb9662c414a9f4c4fa5b3d4a6428b171bba0f1f701da31aefcafa8ef34a9142ea381ced089769332080da3bab7e34da39f5c29087ef6ce6cad5847aa05ab563a8f0938dbace2cd4da1456f841c754107d3887d7d3b253306e08ddbeaea85c4f25e45ffa0fa06e132f2269746c90dd41199da1de78d9b9f6bbb2a48b9775623ab6d256fb8dcfde036633eef0bd7984b094fe318bef2ef364cfe73c2014e353dd66a9552a56d707a5609fbd62274216ba50a22978972f852ea6977f2029f7dfed01d1eab95e58e60348e40618e9ef363df4f02d53128cb1b8cc2d5f85356b0a833b888a8e5668f2b68b948e8cd09e3677a1bdcb521976c7268bd951a99d280fd0ebfc9b8630ce581d789b092230d5788433a418181caa6236cc02b107fca64d8181340a395c01c072f439bd63f8baa395514952396e0f092c66a7e8c73a88b082cf309f9631240ad33c0a6884edd407b5003054479c228d29869224e17efe0cee0428f6e268ab6378571b983c916f8891a3e2e9f6ea7ff63585bd43f0a88e99906362c10b97f19cc0f9dc87f67389b130ce3d0abc1d521fa5a32c21cf0e6533a2fc374811c846a9f4a2230454642c0abb3241b7bcd7c5ee659be53d0817cb94d5f21b51b58b094e726a93ab256cb430689dd4966d2f9ee2d25ce16445401c539567e2a5a808c84d0850330f1b4011a54413b7f8b44c1f013d1f2a85ecefc767d872402c4e67943afc5207905f5c7ef6bd891944d7c20d9c2afc79b75b289e90dd0cc3ce8077bca2f4497e5f58ccb7fbc02af08e6f3bdc78a258953c08bc223c667e3c51b5a8e0d7c8f4668b968a91531b5369ee973a89e8eb54ddd7b731bbf6bc18330148c156d8abd9ce2cbf83122a403dc187244a82bdd2bca902755baa3f64a470170d41e06b4e7fa5d3185ae13db117a52998445bb3ae8939eef408bc36f2190f46b7ce06fcc1f50d7211691e38fb199f889d310bb82e0ff5dd47ef550d5ac91f96bf53b0d4b95f4534658b350e8260810fc3e51c333550535c63a4afc6d00f191a28374860a1adb4c5d0f8fd3242466482909b9eb9d6ff04050c16203d3e44465159676a718b98a4a8b6ced7d8d9fd00000000000000000000000000000000000000000b19243c
verified   : true
```
