---
title: Distance between 3D Ray and Plane in Ray-Plane Order (半直線・平面順の三次元距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/ray_plane/ray_plane_distance_ray_plane.hpp
---

## API

- `distance(ray, plane)`: `Ray3` と `Plane3` の集合間距離を返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

交差または重なる場合は0を返す。退化半直線・零法線平面・非有限入力には `std::invalid_argument` を送出する。
