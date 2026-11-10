---
title: Binomial Coefficient Modulo a Composite (合成数法の二項係数)
documentation_of: ../../../../src/algorithm/math/combinatorics/binomial_mod_composite.hpp
---

任意の正整数 $m$ に対して $\binom{n}{k}\bmod m$ を求める。$m$ を素数冪に分解し、各素数冪を法とする値を一般化 Lucas の定理で求めて、中国剰余定理で統合する。

## API

```cpp
math::u64 math::binomial_mod_composite(
    math::u64 n, math::u64 k, math::u64 modulus
);
```

`modulus == 1` または `k > n` なら `0` を返す。

## 時間計算量

$K = \min(k, n-k)$ とする。Pollard–Rho 法による 64 ビット整数の素因数分解の時間に加え、各素数冪 $P_i$ について、$P_i \le K$ なら $O(\min(K, P_i)\log n + P_i)$、そうでなければ $O(\min(K, P_i)\log n)$。

## 注意点

`modulus` は正でなければならず、`0` では `runtime_error` を送出する。中国剰余定理での統合の途中で法の積が `u64` を超える場合は `overflow_error` を送出する（`modulus` 自身が `u64` なので通常は起きない）。
