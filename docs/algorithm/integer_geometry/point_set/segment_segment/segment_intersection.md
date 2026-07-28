---
title: Exact Segment Intersection Set (厳密線分交差集合)
documentation_of: ../../../../../src/algorithm/integer_geometry/point_set/segment_segment/segment_intersection.hpp
---

二閉線分の共通部分を0点、1点、または重なり区間の両端点で返す。

## API

- `segment_intersection(first, second)`: 重複のない共有点列を辞書順で返す。

## API別の時間計算量・空間計算量

- `segment_intersection(first, second)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
