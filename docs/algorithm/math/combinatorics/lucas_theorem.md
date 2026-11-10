---
title: Lucas Theorem (Lucasの定理)
documentation_of: ../../../../src/algorithm/math/combinatorics/lucas_theorem.hpp
---

素数 $p$ に対する二項係数 $\binom{n}{k}\bmod p$ を求める。

## API

```cpp
math::u64 math::lucas_binomial_mod_prime(
    math::u64 n, math::u64 k, math::u64 prime
);
```

$\binom{n}{k} \bmod p$（`prime` が $p$）を返す。`k > n` なら `0` を返す。

## 時間計算量

$n, k$ の $p$ 進表記の桁数を $L$、各桁を $n_i, k_i$、$S = \sum_i \min(k_i, n_i - k_i)$ とする。

- 各桁の二項係数: $O(\min(k_i, n_i - k_i) + \log p)$
- `lucas_binomial_mod_prime` 全体: $O(S + L \log p)$
- 追加空間: $O(1)$

## 注意点

- `prime` は素数でなければならず（Miller-Rabin で判定）、違反時は `runtime_error` を送出する。
- 各桁で $\binom{n_i}{k_i}$ を直接計算し、Fermat の小定理で分母の逆元を求めて Lucas の定理を適用する。前計算表は使わないため、$p$ が大きく桁 $n_i, k_i$ も大きいと遅くなる。
- 乗算剰余は `__uint128_t` を用いて `unsigned long long` の全域で正確に計算する。
