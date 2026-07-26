---
title: Exact Squared Distances (厳密距離二乗)
documentation_of: ../../../src/integer_geometry/2d/distance.hpp
---

平方根を使わず、距離の二乗を有理数として厳密に計算する。

## API

- `squared_norm(vector)`: ベクトルの長さの二乗を返す。
- `squared_distance(first, second)`: 2点間距離の二乗を返す。
- `closest_pair_squared(points)`: 最近点対距離の二乗を返す。2点未満なら `std::nullopt`。
- `convex_diameter_squared(convex_polygon)`: 凸多角形の直径の二乗を返す。空なら `std::nullopt`。
- `point_set_diameter_squared(points)`: 有限点集合の直径の二乗を返す。空なら `std::nullopt`。

## API別の時間計算量・空間計算量

点数を $n$、有理数比較・演算時間を $R(L)$ とする。

- `squared_norm`, `squared_distance`: 時間 $O(R(L))$、追加領域 $O(L)$。
- `closest_pair_squared`: 時間 $O(n\log n\,R(L))$、領域 $O(nL)$。
- `convex_diameter_squared`: 時間 $O(nR(L))$、追加領域 $O(L)$。
- `point_set_diameter_squared`: 時間 $O(n\log n\,R(L))$、領域 $O(nL)$。

## 注意点

距離そのものではなく二乗を返す。凸直径の入力は境界順の凸多角形でなければならない。退化した点集合と共線凸包を扱える。
