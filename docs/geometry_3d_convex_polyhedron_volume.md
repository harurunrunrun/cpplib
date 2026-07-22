---
title: Convex Polyhedron Volume (凸多面体の体積)
documentation_of: ../src/algorithm/geometry/3d/scalar/convex_polyhedron_volume.hpp
---

## API

- `convex_polyhedron_volume(polyhedron)`: 向き付き体積の絶対値を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間 $O(V+F)$、追加領域 $O(1)$。

## 注意点

閉じた一貫した向きの三角形面を前提とする。入力検査と例外は `convex_polyhedron_signed_volume` に従う。
