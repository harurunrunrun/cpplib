---
title: Exact Polygon Area (厳密多角形面積)
documentation_of: ../../../../../src/algorithm/integer_geometry/scalar/point_set/polygon_area.hpp
---

単純多角形の符号付き面積と絶対面積を有理数で計算する。

## API

- `polygon_twice_signed_area(polygon)`: 符号付き面積の2倍を返す。
- `polygon_twice_area(polygon)`: 面積の2倍を返す。
- `polygon_signed_area(polygon)`: 符号付き面積を返す。
- `polygon_area(polygon)`: 面積を返す。

## API別の時間計算量・空間計算量

- `polygon_twice_signed_area(polygon)`: 時間 O(n)、追加領域 O(1)。
- `polygon_twice_area(polygon)`: 時間 O(n)、追加領域 O(1)。
- `polygon_signed_area(polygon)`: 時間 O(n)、追加領域 O(1)。
- `polygon_area(polygon)`: 時間 O(n)、追加領域 O(1)。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した回数に加え、座標のbit長に応じた多倍長整数演算コストが掛かる。
