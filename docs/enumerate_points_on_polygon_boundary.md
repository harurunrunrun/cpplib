---
title: Enumerate Points on Polygon Boundary (多角形境界上の点列挙)
documentation_of: ../src/algorithm/geometry/2d/polygon_boundary_points/enumerate_points_on_polygon_boundary.hpp
---

## API

- `enumerate_points_on_polygon_boundary(polygon, points, unique = true)`: 多角形の辺上にある `points` のindexを辺順・辺上の進行順で返す。

`unique` が真なら各indexは最初の出現だけ、偽なら頂点上の点などを複数辺について返す。

## API別の時間計算量・空間計算量

- 辺数を $N$、点数を $M$、出力長を $K$ として、時間 $O(NM\log M+K)$、追加領域 $O(M+K)$。

## 注意点

多角形は頂点順に与える。零長辺にも対応し、戻り値は座標でなく入力 `points` のindexである。
