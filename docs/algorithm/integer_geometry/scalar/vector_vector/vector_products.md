---
title: Exact Vector Products (厳密ベクトル積)
documentation_of: ../../../../../src/algorithm/integer_geometry/scalar/vector_vector/vector_products.hpp
---

2 本のベクトルの内積と外積を厳密に計算する。

## API

- `dot(first, second)`: 内積 $x_1 x_2 + y_1 y_2$ を返す。
- `cross(first, second)`: 二次元の外積 $x_1 y_2 - y_1 x_2$ を返す。

## 時間計算量・空間計算量

- `dot`, `cross`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した計算量は有理数演算の回数であり、各演算には座標のビット長に応じた多倍長整数演算のコストが別途掛かる。
