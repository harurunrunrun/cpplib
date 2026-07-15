---
title: Convex Polyhedron Sphere Intersects (凸多面体と球の交差判定)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_sphere_intersects.hpp
---

## API

- `convex_polyhedron_sphere_intersects(polyhedron, sphere)`: 球の内部または表面と閉凸形状が共通点を持つか返す。

## API別の時間計算量・空間計算量

- 面数を $F$ として時間 $O(F)$、追加領域 $O(1)$。

## 注意点

負または非有限半径を拒否する。接触はtrue。
