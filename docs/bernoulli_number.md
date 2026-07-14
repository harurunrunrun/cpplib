---
title: Bernoulli Number (ベルヌーイ数) [bernoulli_number]
documentation_of: ../src/algorithm/math/bernoulli_number.hpp
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

で定義されるBernoulli数 $B_0,\ldots,B_N$ を法 `MOD` で返す。
この規約では $B_1=-1/2$。返却列の長さは `maximum_index + 1`。

$(e^x-1)/x=\sum_{n\geq0}x^n/(n+1)!$ の形式的冪級数逆元を求め、
係数へ $n!$ を掛ける。

- 時間計算量: $O(M(N))$。この実装のNTTでは $O(N\log N)$
- 空間計算量（追加領域）: $O(N)$

$N=\mathtt{maximum\_index}+1$。`maximum_index + 1 < MOD` が必要で、
満たさなければ `length_error`。必要な階乗が逆元を持ち、必要な2冪NTT長が
`MOD - 1` を割り切る必要がある。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
