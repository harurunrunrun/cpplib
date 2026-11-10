---
title: Polynomial Middle Product (多項式中間積)
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_middle_product.hpp
---

連続する係数の窓と `kernel` の内積を、反転した `kernel` との畳み込みとして一括で計算する。

## API

- `math::polynomial_middle_product<MOD>(values, kernel)`: 各 $i$ について $\sum_j \mathrm{values}_{i+j} \, \mathrm{kernel}_j$ を返す。返す列の長さは `values.size() - kernel.size() + 1` である。

## API別の時間計算量・空間計算量

入力長の和を $N$、多項式乗算の時間を $M(N)$ とすると、時間計算量は $O(M(N))$、追加領域は $O(N)$ である。

## 注意点

- `kernel` が空の場合は `std::invalid_argument` を投げる。
- `values` が `kernel` より短い場合は空の列を返す。
