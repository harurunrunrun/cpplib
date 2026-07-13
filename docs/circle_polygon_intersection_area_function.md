---
title: Circle-Polygon Intersection Area Function (円と多角形の共通部分面積関数)
documentation_of: ../src/algorithm/geometry/2d/circle_polygon_intersection_area/circle_polygon_intersection_area.hpp
---

## API

- `circle_polygon_intersection_area(circle, polygon)`: 円盤と単純多角形の共通部分の非負面積を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間 $O(N)$、追加領域 $O(1)$。

## 注意点

多角形は時計回り・反時計回りのどちらでもよい。負半径には `std::invalid_argument` を送出する。
