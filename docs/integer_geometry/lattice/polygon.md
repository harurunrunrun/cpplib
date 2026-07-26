---
title: Exact Lattice Polygon (厳密格子多角形)
documentation_of: ../../../src/integer_geometry/lattice/polygon.hpp
---

任意精度整数格子上の多角形について面積、境界格子点数、内部格子点数を厳密に求める。

## API

- `lattice_polygon_twice_signed_area(polygon)`: 符号付き面積の2倍を返す。
- `lattice_polygon_twice_area(polygon)`: 面積の2倍を返す。
- `lattice_boundary_points(polygon)`: 境界上の異なる格子点総数を返す。
- `lattice_interior_points(polygon)`: Pickの定理で内部格子点数を返す。
- `LatticePointCount{boundary, interior}`: 格子点数の結果型。
- `lattice_point_count(polygon)`: 境界・内部格子点数をまとめて返す。

## API別の時間計算量・空間計算量

頂点数を $n$、座標bit長を $L$、任意精度乗算時間を $M(L)$、最大公約数の時間を $G(L)$ とする。

- 面積API: 時間 $O(nM(L))$、追加領域 $O(L)$。
- `lattice_boundary_points`: 時間 $O(nG(L))$、追加領域 $O(L)$。
- `lattice_interior_points`, `lattice_point_count`: 時間 $O(n(M(L)+G(L)))$、追加領域 $O(L)$。

## 注意点

頂点は格子点で境界順に与える。`lattice_interior_points` は3頂点未満、面積0、またはPickの定理と整合しない入力に `std::invalid_argument` を送出する。
