---
title: Convex Polyhedron Point Distance (凸多面体と点の距離)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_point_distance.hpp
---

## API

- `convex_polyhedron_point_distance(polyhedron, point)`: 点と閉凸形状の最短距離を返し、内部と境界では0を返す。

## API別の時間計算量・空間計算量

- 面数を $F$ として時間 $O(F)$、追加領域 $O(1)$。

## 注意点

空形状には `std::invalid_argument` を送出する。
