---
title: Convex Polyhedron Point Distance (凸多面体と点の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/polyhedron_point/convex_polyhedron_point_distance.hpp
---

## API

- `convex_polyhedron_point_distance(polyhedron, point)`: 点と閉じた凸形状の最短距離を返す。点が内部または境界上にあれば $0$ を返す。

## API別の時間計算量・空間計算量

- 面数を $F$ として、時間 $O(F)$、追加領域 $O(1)$。

## 注意点

- `convex_polyhedron_closest_point` で最近点を求め、その点との距離を返す。アフィン次元が 3 未満の退化した形状（点・線分・平面図形）にも対応する。
- 頂点が空の形状や非有限なクエリ点には `std::invalid_argument` を、範囲外の頂点添字には `std::out_of_range` を送出する。
