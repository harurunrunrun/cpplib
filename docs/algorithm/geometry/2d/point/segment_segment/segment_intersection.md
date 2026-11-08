---
title: 2D Segment Intersection Point (二次元線分交点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point/segment_segment/segment_intersection.hpp
---

## API

- `Point segment_intersection(first, second)`: 2 本の閉線分のただ 1 つの共通点を返す。

## API別の時間計算量・空間計算量

- `segment_intersection`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 共通点がない場合、または重なりが線分となり共通点が一意に定まらない場合は `std::domain_error` を送出する。
- 退化線分（長さ $0$ の線分）を含む場合にも対応する。
