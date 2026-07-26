---
title: Distance between 3D Plane and Ray in Plane-Ray Order (平面・半直線順の三次元距離)
documentation_of: ../../../../src/algorithm/geometry/3d/ray_plane_distance_plane_ray.hpp
---

## API

- `distance(plane, ray)`: `Plane3` と `Ray3` の集合間距離を返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

逆順leafは `distance(ray, plane)` へ委譲する。返り値と例外条件は順方向leafと同じである。
