---
title: Convex Polyhedron Ray Interval (凸多面体と半直線の交差区間)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_ray_interval.hpp
---

## API

- `convex_polyhedron_ray_interval(polyhedron, ray)`: `origin + t*(through-origin), t>=0` の立体内区間を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間 $O(V+F)$、追加領域 $O(1)$。

## 注意点

交差なしは `std::nullopt`。立体と半直線の前提・例外はline版に従う。
