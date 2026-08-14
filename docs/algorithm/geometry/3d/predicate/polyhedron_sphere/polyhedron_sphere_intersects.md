---
title: General Polyhedron-Sphere Intersection (一般多面体と球の交差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_sphere/polyhedron_sphere_intersects.hpp
---

## API

- `polyhedron_sphere_intersects(polyhedron, sphere)`: 非凸多面体の閉領域と閉球が共有点を持つかを返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ としてAABB構築を含む時間計算量 $O(V+F\log F)$、最近面探索の最悪時間計算量 $O(F)$、追加空間計算量 $O(V+F)$。

## 注意点

負半径・非有限球・空の多面体を拒否する。接する場合も交差とする。
