---
title: Enumerate Points on Polygon Boundary (多角形境界上の点列挙)
documentation_of: ../src/algorithm/geometry/2d/point_collection/enumerate_points_on_polygon_boundary.hpp
---

## API

- `enumerate_points_on_polygon_boundary(polygon, points, unique = true)`: 多角形の辺上にある `points` のindexを辺順・辺上の進行順で返す。

`unique` が真なら各indexは最初の出現だけ、偽なら頂点上の点などを複数辺について返す。

## API別の時間計算量・空間計算量

- 辺数を $N$、点数を $M$、出力長を $K$ として、時間
  $O(NM+K\log(K+1))$、追加領域 $O(M+K)$。

## 注意点

多角形は頂点順に与える。厳密に零長の辺にも対応し、非零の短辺では辺方向parameterを
辺長平方で正規化して進行順を決める。線分上判定には絶対距離
`GEOMETRY_EPS` と機械丸め誤差を用いる。戻り値は座標でなく入力 `points` のindexである。
