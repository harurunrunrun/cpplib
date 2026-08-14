---
title: General Polyhedron-Segment Intersection Predicate (一般多面体と線分の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_segment/polyhedron_segment_intersects.hpp
---

## API

- `polyhedron_segment_intersects(polyhedron, segment)`: 非凸多面体の閉領域と閉線分が共有点を持つかを返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間計算量 $O(V+F)$、追加空間計算量 $O(1)$。

## 注意点

端点包含または任意の三角形面との交差を調べる。凸性は要求しない。閉じた一貫した向きの非退化三角形メッシュと有限線分を前提とする。
