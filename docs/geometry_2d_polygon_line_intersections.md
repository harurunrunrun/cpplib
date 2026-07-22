---
title: Polygon-Line Intersections (多角形と直線の交点)
documentation_of: ../src/algorithm/geometry/2d/point_collection/polygon_line_intersections.hpp
---

## API

`vector<Point> polygon_line_intersections(polygon, line)`

## 引数

多角形の周回順頂点列と非退化直線を渡す。

## 戻り値

交点を直線の `a` から `b` 方向の順に重複なく返す。辺が直線上なら両端点を有限表現として含める。

## API別の時間計算量・空間計算量

- `polygon_line_intersections`: 頂点数を $N$、境界辺から得られる交点候補数を
  $K$ として、時間 $O(N+K\log K)$、追加領域 $O(K)$。候補を直線上の
  parameter順に一括sortしてから、隣接する重複を除く。

## 注意点

退化直線では `std::invalid_argument`。
退化判定に該当しない範囲では、直線の方向ベクトルを正の定数倍しても交点集合と順序は変わらない。
