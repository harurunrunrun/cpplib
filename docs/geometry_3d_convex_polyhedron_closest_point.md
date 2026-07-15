---
title: Convex Polyhedron Closest Point (凸多面体上の最近点)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_closest_point.hpp
---

## API

- `convex_polyhedron_closest_point(polyhedron, point)`: 凸形状内ならquery自身、外なら表面上の最近点を返す。

## API別の時間計算量・空間計算量

- 面数を $F$ として時間 $O(F)$、追加領域 $O(1)$。

## 注意点

空形状には `std::invalid_argument` を送出する。複数の最近点がある場合の選択は規定しない。
