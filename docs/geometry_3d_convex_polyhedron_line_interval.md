---
title: Convex Polyhedron Line Interval (凸多面体と直線の交差区間)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_line_interval.hpp
---

## API

- `convex_polyhedron_line_interval(polyhedron, line)`: `line.a + t*(line.b-line.a)` が立体内となる閉区間 `[t_min,t_max]` を返し、交差しなければ `std::nullopt` を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間 $O(V+F)$、追加領域 $O(1)$。

## 注意点

アフィン次元3、外向き面、非退化直線を前提とする。端点差は正規化後に計算する。
