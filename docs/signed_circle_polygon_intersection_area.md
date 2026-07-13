---
title: Signed Circle-Polygon Intersection Area (円と多角形の符号付き共通部分面積)
documentation_of: ../src/algorithm/geometry/circle_polygon_intersection_area/signed_circle_polygon_intersection_area.hpp
---

## API

- `signed_circle_polygon_intersection_area(circle, polygon)`: 円盤との共通部分の符号付き面積を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間 $O(N)$、追加領域 $O(1)$。

## 注意点

多角形が反時計回りなら正、時計回りなら負となる。単純多角形を頂点順に与え、円の半径は非負でなければならない。
