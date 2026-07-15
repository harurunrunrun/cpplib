---
title: Point to 3D Sphere Distance (点と三次元球面の距離)
documentation_of: ../src/algorithm/geometry/3d/sphere_point_distance.hpp
---

## API

- `distance(sphere, point)`: 点と球面との最短距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

球体ではなく球面への距離である。負半径には `std::invalid_argument` を送出する。
