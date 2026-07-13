---
title: 3D Ray-Sphere Intersections (三次元半直線と球面の交点)
documentation_of: ../src/algorithm/geometry/geometry_3d/ray_sphere_intersections.hpp
---

## API

- `ray_sphere_intersections(ray, sphere)`: 半直線と球面の異なる交点を始点からの順に0～2個返す。

## API別の時間計算量・空間計算量

- 時間・出力・追加領域 $O(1)$。

## 注意点

退化半直線または負半径には `std::invalid_argument` を送出する。
