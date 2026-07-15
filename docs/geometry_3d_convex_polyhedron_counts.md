---
title: Convex Polyhedron Counts (凸多面体の要素数)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_counts.hpp
---

## API

- `ConvexPolyhedronCounts3`: `vertices`, 幾何学的 `edges`, `triangular_faces` を保持する。
- `convex_polyhedron_counts(polyhedron)`: 3種類の要素数を返す。

## API別の時間計算量・空間計算量

- 面数を $F$ として時間 $O(F\log F)$、領域 $O(F)$。

## 注意点

共面三角形間の内部対角線は辺数に含めず、面数は格納された三角形の数である。
