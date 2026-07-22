---
title: Convex Polyhedron Facets (三次元凸多面体の幾何面)
documentation_of: ../src/algorithm/geometry/3d/query/convex_polyhedron_facets.hpp
---

## API

- `ConvexPolyhedronFacet3`: 同一平面上の三角形群について、外向きの境界頂点ループ `boundary` と元の三角形添字列 `triangles` を保持する。
- `convex_polyhedron_facets(polyhedron)`: 三角形分割の内部対角線を除去し、幾何学的な面ごとにまとめる。

## API別の時間計算量・空間計算量

- `convex_polyhedron_facets`: 三角形数を $F$ として、時間 $O(F \log F)$、空間 $O(F)$。

## 注意点

面添字は範囲内で、各幾何面の三角形は一貫した向きに並ぶ必要がある。非多様体辺、退化三角形、開いた境界には `std::domain_error`、範囲外添字には `std::out_of_range` を送出する。アフィン次元が 2 未満なら空列を返す。
