---
title: Binary Expansion Operations (浮動小数点展開の二項演算)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/mixed/geometry3d_expansion_binary.hpp
---

2個の浮動小数点展開に対する和と積を提供します。

## API

- `geometry3d_expansion_sum(first, second)`: 2展開の厳密な和を表す展開を返します。
- `geometry3d_expansion_product(first, second)`: 2展開の厳密な積を表す展開を返します。

## API別の時間計算量・空間計算量

成分数を $N,M$ とすると、`sum` は時間 $O(M(N+M))$、`product` は最悪時間 $O(N^2M^2)$ です。返却値と中間展開の空間は生成される成分数に比例します。

## 注意点

成分は絶対値の小さい順に保持してください。非有限成分は `std::invalid_argument`、演算結果のoverflowは `std::overflow_error` になります。