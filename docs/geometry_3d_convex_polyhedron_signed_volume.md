---
title: Convex Polyhedron Signed Volume (凸多面体の向き付き体積)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_signed_volume.hpp
---

## API

- `convex_polyhedron_signed_volume(polyhedron)`: 向き付き三角形面と基準点が作る四面体の体積和を返す。外向き面では正になる。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間 $O(V+F)$、追加領域 $O(1)$。

## 注意点

閉じた一貫した向きの面を前提とする。アフィン次元3未満は0を返し、overflowは `std::overflow_error` とする。
