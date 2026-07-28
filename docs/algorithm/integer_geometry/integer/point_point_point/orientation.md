---
title: Exact Point Orientation (厳密三点方向判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/integer/point_point_point/orientation.hpp
---

三点の反時計回り・一直線・時計回りを符号で判定する。

## API

- `orientation(first, second, third)`: 反時計回りなら1、一直線なら0、時計回りなら-1を返す。
- `ccw(first, second, third)`: `orientation` と同じ結果を返す別名。

## API別の時間計算量・空間計算量

- `orientation(first, second, third)`: 時間 O(1)、追加領域 O(1)。
- `ccw(first, second, third)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
