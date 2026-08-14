---
title: General Polyhedron Point Distance (一般多面体と点の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/polyhedron_point/polyhedron_point_distance.hpp
---

## API

- `polyhedron_point_distance(polyhedron, point)`: 凸性を仮定しない多面体の閉領域と点の最小Euclidean距離を返す。内部・境界なら $0$。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として、AABB構築を含む時間計算量 $O(V+F\log F)$、最近面探索の最悪時間計算量 $O(F)$、追加空間計算量 $O(V+F)$。

## 注意点

入力の前提・例外は `polyhedron_closest_point` に従う。
