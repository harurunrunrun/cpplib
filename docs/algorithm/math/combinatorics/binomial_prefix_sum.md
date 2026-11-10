---
title: Binomial Prefix Sum (二項係数累積和)
documentation_of: ../../../../src/algorithm/math/combinatorics/binomial_prefix_sum.hpp
---

二項係数とその累積和（$k$ についての先頭からの和）を前計算する。

## テンプレート引数

```cpp
BinomialPrefixSum<Mint, MAX_N>
```

- `Mint`: modint などの値の型
- `MAX_N`: $n$ の最大値。省略すると実行時にサイズを指定する可変長版になる

## コンストラクタ

```cpp
BinomialPrefixSum<Mint, 1000> fixed;
BinomialPrefixSum<Mint> dynamic(n);
```

容量を明示する固定長版は `std::array` に、容量を省略する可変長版は実行時のサイズ `n`（デフォルト $0$）に合わせた `std::vector` に表を持つ。可変長版で `n` が負なら `invalid_argument` を送出する。

## クエリ

```cpp
Mint combination(int n, int k)
Mint prefix_sum(int n, int k)
```

- `combination(n, k)`: $\binom{n}{k}$ を返す。
- `prefix_sum(n, k)`: $\sum_{i=0}^{k} \binom{n}{i}$ を返す。

## 時間計算量・空間計算量

以下では `MAX_N`（可変長版ではコンストラクタに渡した値）を $N$ とし、`Mint` の演算を $O(1)$ とする。

| API | 時間計算量 |
| --- | --- |
| コンストラクタ | $O(N^2)$（前処理） |
| `combination(n, k)` | $O(1)$ |
| `prefix_sum(n, k)` | $O(1)$ |
| 公開表 `comb`, `prefix` の 1 要素の参照 | $O(1)$ |

保持する領域は $O(N^2)$。

## 注意点

- `comb[n][k]` は $\binom{n}{k}$、`prefix[n][k]` は $\sum_{i=0}^{k}\binom{n}{i}$ を格納した公開の前計算表である。
- `combination(n, k)` は $0 \le n \le N$ を要求し、範囲外なら `runtime_error` を送出する。$k < 0$ または $k > n$ では $0$ を返す。
- `prefix_sum(n, k)` は $n$ が範囲外なら `runtime_error` を送出し、$k < 0$ なら $0$ を返す。$k > N$ は $N$ に丸めるため、$k \ge n$ では $2^n$ に一致する。
