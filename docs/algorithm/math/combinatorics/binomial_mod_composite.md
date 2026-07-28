---
title: Binomial Coefficient Modulo a Composite (合成数法の二項係数)
documentation_of: ../../../../src/algorithm/math/combinatorics/binomial_mod_composite.hpp
---

任意の正整数 $m$ に対して $\binom{n}{k}\bmod m$ を求める。$m$ を素数冪へ分解し、各剰余を一般化Lucas法で求めて中国剰余定理で統合する。

## API

```cpp
math::u64 math::binomial_mod_composite(
    math::u64 n, math::u64 k, math::u64 modulus
);
```

`modulus == 1` または `k > n` なら `0` を返す。

## 時間計算量

Pollard--Rho法による64-bit整数の素因数分解時間に加え、各素数冪 $P_i$ について $O(\min(K,P_i)\log n+P_i[\,P_i\le K\,])$。ここで $K=\min(k,n-k)$。

## 注意点

`modulus` は正でなければならず、`0` では `runtime_error` を送出する。
