---
title: Distance between 3D Ray and Plane (三次元半直線と平面の距離)
documentation_of: ../../../../src/algorithm/geometry/3d/ray_plane_distance.hpp
---

## API

- `distance(first, second)`: `Ray3` と `Plane3` の集合間距離を返す。異種型では逆順overloadも提供する。
- この互換集約ヘッダは `ray_plane` 順と `plane_ray` 順の両leafを提供する。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

交差または重なる場合は0を返す。退化半直線・零法線平面・非有限入力には `std::invalid_argument` を送出する。
