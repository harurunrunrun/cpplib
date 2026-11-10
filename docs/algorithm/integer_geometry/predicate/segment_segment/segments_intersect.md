---
title: Exact Segment Intersection Predicate (厳密線分交差判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/predicate/segment_segment/segments_intersect.hpp
---

2 つの閉線分が 1 点以上を共有するかを判定する。

## API

- `bool segments_intersect(const Segment& first, const Segment& second)`: 共有点があれば `true` を返す。`segment_intersection_kind(first, second) != SegmentIntersectionKind::none` と同値である。

## API別の時間計算量・空間計算量

- `segments_intersect(first, second)`: 時間 $O(1)$、追加領域 $O(1)$

## 注意点

- 計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
- すべての API は名前空間 `integer_geometry` に属する。
