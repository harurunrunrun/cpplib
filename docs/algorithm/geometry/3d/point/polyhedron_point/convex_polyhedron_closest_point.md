---
title: Convex Polyhedron Closest Point (凸多面体上の最近点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/polyhedron_point/convex_polyhedron_closest_point.hpp
---

## API

- `convex_polyhedron_closest_point(polyhedron, point)`: `point` が凸形状の内部または境界上なら `point` 自身を、外部なら形状上の最近点を返す。

## API別の時間計算量・空間計算量

- 面数を $F$ として時間 $O(F)$、追加領域 $O(1)$。

## 注意点

- `affine_dimension` が $0$ 以下なら先頭頂点、$1$ なら先頭頂点と末尾頂点を結ぶ線分上の最近点を返す。それ以外は各三角形面上の最近点のうち最も近いものを返す。
- 頂点が空なら `std::invalid_argument`、面の頂点添字が範囲外なら `std::out_of_range` を送出する。
- 最近点が複数ある場合にどれを返すかは規定しない。
