---
title: Segment-Segment Intersection Test (線分同士の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/segment_segment/intersect_segment_segment.hpp
---

2本の閉線分が少なくとも1点を共有するか判定する。

## API

- `intersect(first, second)`: 閉線分 `first` と `second` が交差するなら `true` を返す。

## API別の時間計算量・空間計算量

- `intersect`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

端点接触、共線部分の重なり、長さ0の退化線分を交差として扱う。浮動小数点座標には幾何用の許容誤差を用いる。
