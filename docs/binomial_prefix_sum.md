---
title: Binomial Prefix Sum (二項係数累積和)
documentation_of: ../src/algorithm/math/binomial_prefix_sum.hpp
---

二項係数とその prefix sum を前計算する。

## テンプレート引数

```cpp
BinomialPrefixSum<Mint, MAX_N>
```

- modint などの型 `Mint`
- 最大 `n`

## query

```cpp
Mint combination(int n, int k)
Mint prefix_sum(int n, int k)
```

`prefix_sum(n,k)` は $\sum_{i=0}^{k} \binom{n}{i}$ を返す。

## 時間計算量

- 前計算: $O(N^2)$
- query: $O(1)$

## API別の時間計算量・空間計算量

`Mint` の演算を $O(1)$ とする。

| API | 時間計算量 |
| --- | --- |
| `BinomialPrefixSum()` | $O(MAX_N^2)$（前処理） |
| `combination(n,k)` | $O(1)$ |
| `prefix_sum(n,k)` | $O(1)$ |
| 公開表 `comb` / `prefix` の1要素参照 | $O(1)$ |

保持領域は $O(MAX_N^2)$。

## 注意点

- `comb[n][k]` は $\binom nk$、`prefix[n][k]` は
  $\sum_{i=0}^{k}\binom ni$ の公開前計算表。
- `combination(n,k)` は $0\le n\le MAX_N$ を要求する。
  $k<0$ または $k>n$ では0を返す。
- `prefix_sum(n,k)` は $n$ が範囲外なら例外、$k<0$ なら0。
  $k>MAX_N$ は `MAX_N` に丸めるため、$k\ge n$ では $2^n$ に一致する。
