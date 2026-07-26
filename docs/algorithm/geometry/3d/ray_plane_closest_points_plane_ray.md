---
title: Closest Points for 3D Plane and Ray in Plane-Ray Order (平面・半直線順の三次元最近点対)
documentation_of: ../../../../src/algorithm/geometry/3d/ray_plane_closest_points_plane_ray.hpp
---

## API

- `closest_points(plane, ray)`: `Plane3` 上の点と `Ray3` 上の点を引数順のpairで返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

逆順leafは `closest_points(ray, plane)` の結果を入れ替える。最近点の選択と例外条件は順方向leafと同じである。
