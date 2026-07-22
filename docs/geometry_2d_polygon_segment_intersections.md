---
title: Polygon-Segment Intersections (多角形と線分の交点)
documentation_of: ../src/algorithm/geometry/2d/point_collection/polygon_segment_intersections.hpp
---

## API

`vector<Point> polygon_segment_intersections(polygon, segment)`

## 引数

多角形の周回順頂点列と線分を渡す。

## 戻り値

境界との交点を線分始点から終点方向の順に重複なく返す。辺との重なりは共通部分の端点を含める。

## API別の時間計算量・空間計算量

- `polygon_segment_intersections`: 頂点数を $N$、境界辺から得られる交点候補数を
  $K$ として、時間 $O(N+K\log K)$、追加領域 $O(K)$。重なり区間の端点も
  候補へ加え、線分parameter順のsort後に隣接重複を除く。

## 注意点

退化線分では、その点が境界上なら1点を返す。
