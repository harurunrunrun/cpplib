---
title: Polynomial Roots over Prime Field (素数体上の多項式根) [polynomial_root_finding]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_roots.hpp
---

`math::polynomial_roots<MOD>(f)` は素数体内の相異なる根を昇順で返す。係数は `math::Polynomial<MOD>`。非零定数の根は空、零多項式は例外。

## 時間計算量

`gcd(f,x^MOD-x)` で重複と次数2以上の既約因子を除き、一次因子だけを Cantor--Zassenhaus 法で分割する。多項式乗算時間を $M(N)$ として、期待 $O(M(N)\log(N+2)(\log MOD+\log(N+2)))$ 時間、再帰中間領域を含め $O(N\log(N+2))$ 領域。後者の対数項は各分割での Half-GCD も含む。

類題: 多項式とその平行移動に共通する根を列挙する。先に `polynomial_gcd_half(f,g)` を計算し、結果へこの関数を適用する。

## 注意点

GCC 13 / C++20。法・可逆性・添字範囲は上記の各APIの条件を満たす必要がある。
