---
title: Distance between 3D Plane and Ray in Plane-Ray Order (平面・半直線順の三次元距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/plane_ray/ray_plane_distance_plane_ray.hpp
---

## API

- `distance(plane, ray)`: `Plane3` と `Ray3` の集合間距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 引数の順序を入れ替えて `distance(ray, plane)` に委譲する。戻り値と例外の条件は `distance(ray, plane)` と同じである。
