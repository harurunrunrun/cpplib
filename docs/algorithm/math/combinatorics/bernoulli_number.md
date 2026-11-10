---
title: Bernoulli Number (ベルヌーイ数) [bernoulli_number]
documentation_of: ../../../../src/algorithm/math/combinatorics/bernoulli_number.hpp
---

## `bernoulli_numbers`

```cpp
template<int MOD>
std::vector<Modint<MOD>> bernoulli_numbers(std::size_t maximum_index)
```

母関数

$$
\frac{x}{e^x-1}=\sum_{n\geq0}B_n\frac{x^n}{n!}
$$

で定義される Bernoulli 数 $B_0, \ldots, B_{N-1}$ を法 `MOD` で返す。ここで $N$ は `maximum_index + 1` であり、戻り値の列の長さに等しい。この規約では $B_1 = -1/2$ である。

$(e^x-1)/x=\sum_{n\geq0}x^n/(n+1)!$ の形式的冪級数としての逆元を求め、各係数に $n!$ を掛ける。

- 時間計算量: 長さ $N$ の多項式乗算の時間を $M(N)$ として $O(M(N))$。この実装の NTT では $O(N\log N)$
- 空間計算量（追加領域）: $O(N)$

## 注意点

`maximum_index + 1 < MOD` が必要で、満たさなければ `length_error` を送出する。`maximum_index` が `int` の最大値以上の場合も `length_error` を送出する。必要な階乗が逆元を持ち、必要な長さ（2 の冪）が `MOD - 1` を割り切る必要がある。
