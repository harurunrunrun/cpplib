---
title: Segment-Segment Intersection Test (線分同士の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/segment_segment/intersect_segment_segment.hpp
---

2 本の閉線分が少なくとも 1 点を共有するか判定する。

## API

- `bool intersect(const Segment& first, const Segment& second)`: 閉線分 `first` と `second` が交差するなら `true` を返す。

## API別の時間計算量・空間計算量

- `intersect`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 端点での接触、共線部分の重なり、長さ $0$ の退化線分が他方の上にある場合も交差として扱う。
- 浮動小数点座標の向き判定には、基準線分からの垂直距離に対する許容誤差 `GEOMETRY_EPS` を用いる。
