---
title: Two Square Representations (二平方和の全列挙) [two_square_sum]
documentation_of: ../../../../../src/algorithm/math/number_theory/gaussian/two_square_representations.hpp
---

`math::two_square_representations(n)` は `a*a+b*b=n` を満たす非負整数の順序付き組 `(a,b)` を、辞書順かつ重複なしで返す。`n` は `uint64_t`、`n=0` も扱う。

## 時間計算量

Pollard--Rho で素因数分解し、$p\equiv1\pmod4$ は Cornacchia 法で Gaussian 素数へ分解する。$p\equiv3\pmod4$ の指数が奇数なら解なし。素因数分解後は候補数を $R$ として $O(R\log R+\tau(n))$ 時間、$O(R+\tau(n))$ 領域。座標の中間積には符号付き128 bitを用いる。

類題: 原点からの二乗距離が `n` の格子点を第一象限で列挙する。`a<=b` に絞れば順序を無視した表示も得られる。

## 注意点

GCC 13 / C++20。法・可逆性・添字範囲は上記の各APIの条件を満たす必要がある。
