---
title: Polygon-Line Intersections (多角形と直線の交点)
documentation_of: ../src/algorithm/geometry/2d/polygon_line_intersections.hpp
---

## API

`vector<Point> polygon_line_intersections(polygon, line)`

## 引数

多角形の周回順頂点列と非退化直線を渡す。

## 戻り値

交点を直線の `a` から `b` 方向の順に重複なく返す。辺が直線上なら両端点を有限表現として含める。

## API別の時間計算量・空間計算量

- `polygon_line_intersections`: 頂点数 $N$、交点数 $K$ として時間 $O(NK+K\log K)$、領域 $O(K)$。

## 注意点

退化直線では `std::invalid_argument`。
退化判定に該当しない範囲では、直線の方向ベクトルを正の定数倍しても交点集合と順序は変わらない。
