---
title: Generalized Lucas Theorem (一般化Lucasの定理)
documentation_of: ../../../../src/algorithm/math/combinatorics/generalized_lucas_theorem.hpp
---

素数冪 $p^q$ に対する二項係数 $\binom{n}{k}\bmod p^q$ を、階乗中の $p$ の指数と $p$ を除いた部分に分けて求める。

## API

```cpp
math::u64 math::generalized_lucas_binomial_mod_prime_power(
    math::u64 n,
    math::u64 k,
    math::u64 prime,
    int exponent
);
```

`k > n` なら `0` を返す。

## 時間計算量

$P=p^q$、$K=\min(k,n-k)$ とする。

- $K<P$: $O(K\log_p n+\log P+\log p+q)$ 時間、$O(1)$ 追加空間。
  分母を全て掛けてから1回だけ拡張Euclidで逆元を求める。
- $K\ge P$: $O(P+\log_p n\log n+\log P+\log p+q)$ 時間、
  $O(P)$ 空間。

## 注意点

`prime` は素数、`exponent` は正でなければならない。$p^q$ が `unsigned long long` を超える場合は `overflow_error` を送出する。
