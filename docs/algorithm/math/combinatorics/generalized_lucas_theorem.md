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

$\binom{n}{k} \bmod p^q$ を返す（`prime` が $p$、`exponent` が $q$）。`k > n` なら `0` を返す。

## 時間計算量

$P = p^q$、$K = \min(k, n-k)$ とする。

- $K < P$: $O(K \log_p n + \log P + \log p + q)$ 時間、$O(1)$ 追加空間。分母をすべて掛けてから 1 回だけ拡張 Euclid の互除法で逆元を求める。
- $K \ge P$: $O(P + \log_p n \log n + \log P + \log p + q)$ 時間、$O(P)$ 空間。$p$ と互いに素な $1, \ldots, P$ の累積積の表を作り、$p$ を除いた階乗を再帰的に求める。

## 注意点

- `prime` は素数（Miller-Rabin で判定）、`exponent` は正でなければならず、満たさない場合は `runtime_error` を送出する。
- $p^q$ が `unsigned long long` に収まらない場合は `overflow_error` を送出する。
- $K \ge P$ の場合は長さ $P + 1$ の表を確保するため、$P$ が大きいと時間・メモリが足りなくなる。
