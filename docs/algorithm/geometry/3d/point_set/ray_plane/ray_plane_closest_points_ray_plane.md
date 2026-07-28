---
title: Closest Points for 3D Ray and Plane in Ray-Plane Order (半直線・平面順の三次元最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/ray_plane/ray_plane_closest_points_ray_plane.hpp
---

## API

- `closest_points(ray, plane)`: `Ray3` 上の点と `Plane3` 上の点を引数順のpairで返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

退化半直線・零法線平面・非有限入力には `std::invalid_argument` を送出する。交差時は同一点のpair、非交差時は半直線始点とその平面射影を返す。
