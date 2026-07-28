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

`k > n` なら `0` を返す。

## 時間計算量

$n,k$ の $p$ 進表記の桁数を $L$、各桁を $n_i,k_i$、
$S=\sum_i\min(k_i,n_i-k_i)$ とする。

- 各桁の二項係数: $O(\min(k_i,n_i-k_i)+\log p)$
- `lucas_binomial_mod_prime` 全体: $O(S+L\log p)$
- 追加空間: $O(1)$

## 注意点

`prime` は素数でなければならず、違反時は `runtime_error` を送出する。
各桁で直接 $\binom{n_i}{k_i}$ を計算して Lucas の定理を適用するため、
素数冪版の実装や前計算表には依存しない。演算は `unsigned long long`
全域で `__uint128_t` により乗算剰余を正確に計算する。
