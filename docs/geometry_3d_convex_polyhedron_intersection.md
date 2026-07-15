---
title: Convex Polyhedron Intersection (凸多面体同士の共通部分)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_intersection.hpp
---

## API

- `convex_polyhedron_intersection(first, second)`: 内包頂点と相手でclipした全辺端点の凸包として共通部分を返す。

## API別の時間計算量・空間計算量

- 辺数を $E_1,E_2$、面数を $F_1,F_2$、候補点数を $K$ として時間 $O(E_1F_2+E_2F_1+K^2)$、領域 $O(K)$。

## 注意点

両入力は外向き面を持つ3次元凸多面体でなければならない。空なら空の `ConvexPolyhedron3` を返す。
