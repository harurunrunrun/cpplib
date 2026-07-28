---
title: Exact Squared Point Distance (厳密点間距離二乗)
documentation_of: ../../../../../src/algorithm/integer_geometry/scalar/point_point/squared_distance.hpp
---

二点間のEuclid距離の二乗を返す。

## API

- `squared_distance(first, second)`: 距離の二乗を返す。

## API別の時間計算量・空間計算量

- `squared_distance(first, second)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
