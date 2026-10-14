---
title: One Modular Power Root (素数法での冪根) [kth_root_mod]
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/one_kth_root_mod_prime.hpp
---

`math::one_kth_root_mod_prime(value, exponent, prime)` は `x^exponent=value mod prime` を満たす根を1つ `optional<uint64_t>` で返す。解なしは `nullopt`。法は素数で、指数0も扱う (`0^0=1`)。

## 時間計算量

まず $g=\gcd(k,p-1)$ による存在判定を行う。$g$ の各素因数冪について、互いに素な部分の指数を逆元で処理し、必要な Sylow 部分だけを位ごとに持ち上げる。$q^e\mid g$ に対して $q^{e+1}\mid p-1$ のときだけ、位 $q$ の部分群で $O(\sqrt q)$ の baby-step giant-step を行う。素因数分解には Pollard--Rho を用いる。全根の列挙はしない。

類題: 巨大な指数の根を求め、存在判定だけ・任意の根だけを返す。全根を必要とする用途には既存の `modular_kth_roots_prime` がある。

## 注意点

GCC 13 / C++20。法・可逆性・添字範囲は上記の各APIの条件を満たす必要がある。
