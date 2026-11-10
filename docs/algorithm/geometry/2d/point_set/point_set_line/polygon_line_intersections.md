---
title: Polygon-Line Intersections (多角形と直線の交点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/point_set_line/polygon_line_intersections.hpp
---

## API

`std::vector<Point> polygon_line_intersections(polygon, line)`

## 引数

多角形の頂点を周回順に並べた列 `polygon` と、非退化な直線 `line` を渡す。

## 戻り値

多角形の境界と直線の交点を、直線上で `line.a` から `line.b` へ向かう順に重複なく返す。辺が直線上にある場合は、その辺の両端点を交点として含める（重なり全体ではなく端点で表す）。

## API別の時間計算量・空間計算量

- `polygon_line_intersections`: 頂点数を $N$、境界の辺から得られる交点候補の個数を $K$ として、時間 $O(N + K \log K)$、追加領域 $O(K)$。候補を直線上のパラメータ順にまとめてソートしてから、隣接する重複を除く。

## 注意点

- `line` の方向ベクトルの長さが `GEOMETRY_EPS` 以下なら退化直線とみなし、`std::invalid_argument` を送出する。
- 方向ベクトルを単位化して判定するため、退化判定に該当しない範囲では、直線の方向ベクトルを正の定数倍しても交点集合とその順序は変わらない。
