---
title: Convex Polyhedron Segment Intersection (凸多面体と線分の共通部分)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_segment_intersection.hpp
---

## API

- `convex_polyhedron_segment_intersection(polyhedron, segment)`: 線分と立体の共通線分を返す。接点は両端が等しい線分で表す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間 $O(V+F)$、追加領域 $O(1)$。

## 注意点

交差なしは `std::nullopt`。退化線分は点包含として扱う。
