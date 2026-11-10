---
title: Bell Numbers (Bell数) [bell_number]
documentation_of: ../../../../src/algorithm/math/combinatorics/bell_numbers.hpp
---

## `math::bell_numbers`

```cpp
template<int MOD>
std::vector<Modint<MOD>> math::bell_numbers(std::size_t maximum_index)
```

`maximum_index` を $N$ とする。$n$ 元集合の集合分割の個数である Bell 数 $B_0, \ldots, B_N$ を返す。指数型母関数

$$
\sum_{n\geq0}B_n\frac{x^n}{n!}=\exp(e^x-1)
$$

を形式的冪級数として計算する。

- 時間計算量: 長さ $N$ の多項式乗算の時間を $M(N)$ として $O(M(N)\log N)$。現在の FPS 実装では $O(N\log^2 N)$
- 空間計算量: $O(N)$

## 注意点

`maximum_index < MOD` が必要で、満たさなければ `length_error` を送出する。法は、必要な階乗が可逆で、必要な長さ（2 の冪）の NTT が可能、すなわちその長さが `MOD - 1` を割り切る素数であることを想定する。
