---
title: 3D Plane-Sphere Intersection (三次元平面と球面の交円)
documentation_of: ../src/algorithm/geometry/geometry_3d/plane_sphere_intersection.hpp
---

## API

- `plane_sphere_intersection(plane, sphere)`: 交円を返し、交わらなければ `nullopt`。接する場合は半径0の円を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

零法線または負半径には `std::invalid_argument` を送出する。
