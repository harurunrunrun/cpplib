---
title: Convex Polyhedron Centroid (凸多面体の重心)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_centroid.hpp
---

## API

- `convex_polyhedron_centroid(polyhedron)`: 点・線分・平面凸多角形・立体凸多面体の次元に応じた重心を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間 $O(V+F)$、領域 $O(V)$。

## 注意点

空集合、面積または体積が0の不整合入力を拒否する。立体では閉じた一貫した向きの面を前提とする。
