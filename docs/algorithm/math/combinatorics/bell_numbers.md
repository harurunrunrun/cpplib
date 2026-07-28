---
title: Bell Numbers (Bell数) [bell_number]
documentation_of: ../../../../src/algorithm/math/combinatorics/bell_numbers.hpp
---

## `math::bell_numbers`

```cpp
template<int MOD>
std::vector<Modint<MOD>> math::bell_numbers(std::size_t maximum_index)
```

$n$ 元集合の集合分割数であるBell数
$B_0,\ldots,B_{\mathtt{maximum\_index}}$ を返す。指数型母関数

$$
\sum_{n\geq0}B_n\frac{x^n}{n!}=\exp(e^x-1)
$$

を形式的冪級数として計算する。

- 時間計算量: $O(M(N)\log N)$。現在のFPS実装では $O(N\log^2 N)$
- 空間計算量: $O(N)$

## 注意点

`maximum_index < MOD` が必要である。法は必要な階乗が可逆で、
必要な2冪NTT長が `MOD - 1` を割り切る素数を想定する。
