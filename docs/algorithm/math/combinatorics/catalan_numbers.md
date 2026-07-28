---
title: Catalan Numbers (Catalan数)
documentation_of: ../../../../src/algorithm/math/combinatorics/catalan_numbers.hpp
---

Catalan数 $C_i=\frac{1}{i+1}\binom{2i}{i}$ を先頭から列挙する。

## API

```cpp
template<int MOD>
vector<Modint<MOD>> math::catalan_numbers(size_t maximum_index);
```

`C_0` から `C_maximum_index` までを返す。

## 時間計算量

$O(N)$ 時間、$O(N)$ 空間。

## 注意点

漸化式の除算を行うため `2 * maximum_index + 1 < MOD` かつ `MOD` が素数であることを前提とする。添字条件の違反では `length_error` を送出する。
