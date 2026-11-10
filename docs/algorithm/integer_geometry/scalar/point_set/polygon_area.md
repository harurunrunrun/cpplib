---
title: Exact Polygon Area (厳密多角形面積)
documentation_of: ../../../../../src/algorithm/integer_geometry/scalar/point_set/polygon_area.hpp
---

単純多角形の符号付き面積と面積を有理数で計算する。符号付き面積は頂点が反時計回りのとき正になる。

## API

- `polygon_twice_signed_area(polygon)`: 符号付き面積の $2$ 倍を返す。
- `polygon_twice_area(polygon)`: 面積の $2$ 倍を返す。
- `polygon_signed_area(polygon)`: 符号付き面積を返す。
- `polygon_area(polygon)`: 面積を返す。

## 時間計算量・空間計算量

頂点数を $n$ とする。

- いずれの関数も時間 $O(n)$、追加領域 $O(1)$。

## 注意点

計算は任意精度整数と既約有理数で厳密に行う。記載した計算量は有理数演算の回数であり、各演算には座標のビット長に応じた多倍長整数演算のコストが別途掛かる。
