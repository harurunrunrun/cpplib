---
title: Binomial Prefix Sum
documentation_of: ../src/algorithm/math/binomial_prefix_sum.hpp
---

二項係数とその prefix sum を前計算する。

# テンプレート引数

```cpp
BinomialPrefixSum<Mint, MAX_N>
```

- modint などの型 `Mint`
- 最大 `n`

# query

```cpp
Mint combination(int n, int k)
Mint prefix_sum(int n, int k)
```

`prefix_sum(n,k)` は $\sum_{i=0}^{k} \binom{n}{i}$ を返す。

## 時間計算量

- 前計算: $O(N^2)$
- query: $O(1)$
