---
title: Lattice Polygon Boundary Points (格子多角形の境界格子点数)
documentation_of: ../src/algorithm/geometry/2d/scalar/lattice_polygon_boundary_points.hpp
---

## API

`__uint128_t lattice_polygon_boundary_points(polygon)`

## 引数

整数座標多角形の周回順頂点列を渡す。

## 戻り値

境界上の異なる格子点数。

## API別の時間計算量・空間計算量

- `lattice_polygon_boundary_points`: 頂点数 $N$ として時間 $O(N\log C)$、追加領域 $O(1)$。

## 注意点

座標差は `long long` の端点間でもoverflowしない。単純多角形を前提とする。
