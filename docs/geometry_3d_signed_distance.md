---
title: Signed Distance to 3D Plane (三次元平面への符号付き距離)
documentation_of: ../src/algorithm/geometry/geometry_3d/signed_distance.hpp
---

## API

- `signed_distance(plane, point)`: 入力法線の向きを正とする平面への符号付き距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

零法線には `std::invalid_argument` を送出する。
