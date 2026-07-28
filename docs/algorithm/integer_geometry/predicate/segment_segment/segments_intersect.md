---
title: Exact Segment Intersection Predicate (厳密線分交差判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/predicate/segment_segment/segments_intersect.hpp
---

二閉線分が一点以上を共有するか判定する。

## API

- `segments_intersect(first, second)`: 共有点があれば真を返す。

## API別の時間計算量・空間計算量

- `segments_intersect(first, second)`: 時間 O(1)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
