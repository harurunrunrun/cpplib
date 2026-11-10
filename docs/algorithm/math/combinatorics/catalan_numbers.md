---
title: Catalan Numbers (Catalan数)
documentation_of: ../../../../src/algorithm/math/combinatorics/catalan_numbers.hpp
---

Catalan 数 $C_i=\frac{1}{i+1}\binom{2i}{i}$ を先頭から列挙する。

## API

```cpp
template<int MOD>
vector<Modint<MOD>> math::catalan_numbers(size_t maximum_index);
```

`maximum_index` を $N$ として、$C_0$ から $C_N$ までを返す。漸化式 $C_{i+1} = C_i \cdot (4i+2) / (i+2)$ で計算する。

## 時間計算量

時間 $O(N)$（各項で除算を 1 回行うため、厳密には $O(N \log \mathit{MOD})$）、空間 $O(N)$。

## 注意点

漸化式で除算を行うため、`2 * maximum_index + 1 < MOD` かつ `MOD` が素数であることを前提とする。添字の条件に違反した場合は `length_error` を送出する。
