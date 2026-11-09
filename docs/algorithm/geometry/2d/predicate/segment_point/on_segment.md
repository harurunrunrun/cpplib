---
title: 2D Point on Segment (二次元線分上判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/segment_point/on_segment.hpp
---

## API

- `bool on_segment(const Segment& segment, const Point& point)`: `point` が閉線分 `segment` 上にあるか判定する。

## API別の時間計算量・空間計算量

- `on_segment`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 端点を含む。長さが `GEOMETRY_EPS` 以下の退化線分は 1 点 `segment.a` として扱う。
- 線分方向に対する垂直距離と、線分上への射影の位置を `GEOMETRY_EPS` で判定する。そのため、生の外積・内積に絶対誤差を適用した場合のような、線分の長さ（スケール）への依存はない。
