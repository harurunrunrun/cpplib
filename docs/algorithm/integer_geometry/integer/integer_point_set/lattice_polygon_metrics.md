---
title: Exact Lattice Polygon Metrics (厳密格子多角形指標)
documentation_of: ../../../../../src/algorithm/integer_geometry/integer/integer_point_set/lattice_polygon_metrics.hpp
---

格子多角形の面積、境界格子点数、内部格子点数を計算する。

## API

- `lattice_polygon_twice_signed_area(polygon)`: 符号付き面積の2倍を返す。
- `lattice_polygon_twice_area(polygon)`: 面積の2倍を返す。
- `lattice_boundary_points(polygon)`: 境界上の格子点数を返す。
- `lattice_interior_points(polygon)`: Pickの定理で内部格子点数を返す。

## API別の時間計算量・空間計算量

- `lattice_polygon_twice_signed_area(polygon)`: 時間 O(n)、追加領域 O(1)。
- `lattice_polygon_twice_area(polygon)`: 時間 O(n)、追加領域 O(1)。
- `lattice_boundary_points(polygon)`: 時間 O(n log C)、追加領域 O(1)。
- `lattice_interior_points(polygon)`: 時間 O(n log C)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。 多角形は頂点数3以上の単純格子多角形とする。
