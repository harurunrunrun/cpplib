---
title: Exact Lattice Polygon Metrics (厳密格子多角形指標)
documentation_of: ../../../../../src/algorithm/integer_geometry/integer/integer_point_set/lattice_polygon_metrics.hpp
---

格子多角形の面積、境界上の格子点数、内部の格子点数を計算する。

## API

- `lattice_polygon_twice_signed_area(polygon)`: 符号付き面積の 2 倍を返す（反時計回りなら正）。
- `lattice_polygon_twice_area(polygon)`: 面積の 2 倍を返す。
- `lattice_boundary_points(polygon)`: 境界上の格子点数を返す。各辺について座標差の絶対値の最大公約数を足し合わせる。
- `lattice_interior_points(polygon)`: Pick の定理で内部の格子点数を返す。

## API別の時間計算量・空間計算量

- `lattice_polygon_twice_signed_area(polygon)`: 時間 $O(n)$、追加領域 $O(1)$
- `lattice_polygon_twice_area(polygon)`: 時間 $O(n)$、追加領域 $O(1)$
- `lattice_boundary_points(polygon)`: 時間 $O(n \log C)$、追加領域 $O(1)$
- `lattice_interior_points(polygon)`: 時間 $O(n \log C)$、追加領域 $O(1)$
- $n$ は頂点数、$C$ は座標の絶対値の上界である。

## 注意点

- 計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
- すべての API は名前空間 `integer_geometry` に属する。
- 引数 `polygon` は `std::vector<IntegerPoint>` で、戻り値は `Integer` である。
- 多角形は頂点数 3 以上の単純な格子多角形とする。`lattice_interior_points` は、頂点数が 3 未満の場合、面積が 0 の場合、または Pick の定理から得られる値が非負整数にならない場合に `invalid_argument` を送出する。
