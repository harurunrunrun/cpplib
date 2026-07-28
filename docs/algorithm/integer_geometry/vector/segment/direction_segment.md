---
title: Segment Direction Vector (線分方向ベクトル)
documentation_of: ../../../../../src/algorithm/integer_geometry/vector/segment/direction_segment.hpp
---

線分の終点と始点の差を返す。

## API

- `direction(Segment)`: 線分の方向ベクトルを返す。

## API別の時間計算量・空間計算量

- `direction(Segment)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
