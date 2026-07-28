---
title: Polynomial Middle Product (多項式中間積)
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_middle_product.hpp
---

連続する係数窓とkernelの内積を、反転畳み込みとして一括計算する。

## API

- `polynomial_middle_product<MOD>(values, kernel)`: 各 $i$ について
  $\sum_j values[i+j]kernel[j]$ を返す。返却要素数は
  `values.size() - kernel.size() + 1`。

## API別の時間計算量・空間計算量

入力長の和を $N$、多項式乗算時間を $M(N)$ とすると、時間計算量は
$O(M(N))$、追加領域は $O(N)$。

## 注意点

`kernel` は空であってはならず、空の場合は `std::invalid_argument`。
`values` が `kernel` より短い場合は空vectorを返す。
