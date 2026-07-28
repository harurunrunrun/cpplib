---
title: Exact Point Addition (厳密点加算)
documentation_of: ../../../../../src/algorithm/integer_geometry/point/point_point/add_points.hpp
---

二点の座標ごとの和を返す。

## API

- `add_points(first, second)`: Minkowski和などで使う座標和を返す。

## API別の時間計算量・空間計算量

- `add_points(first, second)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
