---
title: Exact Vector Products (厳密ベクトル積)
documentation_of: ../../../../../src/algorithm/integer_geometry/scalar/vector_vector/vector_products.hpp
---

二つのベクトルの内積と外積を厳密に計算する。

## API

- `dot(first, second)`: 内積を返す。
- `cross(first, second)`: 二次元外積を返す。

## API別の時間計算量・空間計算量

- `dot(first, second)`: 時間 O(1)、追加領域 O(1)。
- `cross(first, second)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
