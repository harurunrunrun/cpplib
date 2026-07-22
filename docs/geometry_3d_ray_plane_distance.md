---
title: Distance between 3D Ray and Plane (三次元半直線と平面の距離)
documentation_of: ../src/algorithm/geometry/3d/scalar/ray_plane_distance.hpp
---

## API

- `distance(first, second)`: `Ray3` と `Plane3` の集合間距離を返す。異種型では逆順overloadも提供する。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

交差または重なる場合は0を返す。退化した直線・半直線または零法線の平面には `std::invalid_argument` を送出し、退化線分には対応する。
