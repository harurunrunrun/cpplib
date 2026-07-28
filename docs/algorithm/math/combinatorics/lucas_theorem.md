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

$K=\min(k,n-k)$ とする。$K<p$ なら $O(K\log p)$ 時間・$O(1)$ 追加空間、それ以外は $O(p+\log_p n)$ 時間・$O(p)$ 空間。

## 注意点

`prime` は素数でなければならず、違反時は `runtime_error` を送出する。演算は `unsigned long long` 全域で `__uint128_t` により乗算剰余を正確に計算する。
