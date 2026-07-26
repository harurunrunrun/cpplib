---
title: Exact Rational Polygon (厳密有理数多角形)
documentation_of: ../../../src/integer_geometry/2d/polygon.hpp
---

有理数座標の多角形について面積と点包含を誤差なく求める。

## API

- `polygon_twice_signed_area(polygon)`: 符号付き面積の2倍を返す。
- `polygon_twice_area(polygon)`: 面積の2倍を返す。
- `polygon_signed_area(polygon)`: 符号付き面積を返す。
- `polygon_area(polygon)`: 面積を返す。
- `point_in_polygon(polygon, point)`: 点を外部・境界・内部に分類する。

## API別の時間計算量・空間計算量

頂点数を $n$、座標の最大bit長を $L$、有理数演算時間を $R(L)$ とする。

- 面積APIと `point_in_polygon`: 時間 $O(nR(L))$、追加領域 $O(L)$。

## 注意点

頂点は境界順に与える。自己交差多角形の `point_in_polygon` は非零巻き数規則になる。空多角形に対する包含判定は `outside` を返す。
