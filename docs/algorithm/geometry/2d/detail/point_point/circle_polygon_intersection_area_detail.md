---
title: Circle-Polygon Intersection Area Internals (円と多角形の共通部分面積の内部関数)
documentation_of: ../../../../../../src/algorithm/geometry/2d/detail/point_point/circle_polygon_intersection_area_detail.hpp
---

## API

- `circle_polygon_intersection_internal::edge_area(a, b, radius)`: 原点を中心とする半径 `radius` の円と三角形（原点、`a`、`b`）の共通部分の符号付き面積を返す。多角形の各有向辺についてこの値を足し合わせると、円と多角形の共通部分の面積になる。

## API別の時間計算量・空間計算量

- `edge_area`: 時間・追加領域とも $O(1)$。

## 注意点

上位の面積 API 向けの内部関数であり、互換性は保証しない。`radius` は非負で、座標と中間演算は有限であることを前提とする。
`a` と `b` が完全に一致する場合は 0 を返す。円周との交点の判定には相対許容誤差を、交点パラメータの重複除去には `GEOMETRY_EPS` を用いる。
