---
title: Exact Oriented Area Product (厳密有向面積積)
documentation_of: ../../../../../src/algorithm/integer_geometry/scalar/point_point_point/cross_points.hpp
---

三点が作る二本のベクトルの外積を返す。

## API

- `cross(first, second, third)`: `(second-first)×(third-first)` を返す。

## API別の時間計算量・空間計算量

- `cross(first, second, third)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
