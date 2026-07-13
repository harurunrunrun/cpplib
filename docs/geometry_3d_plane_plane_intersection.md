---
title: 3D Plane-Plane Intersection (三次元平面同士の交線)
documentation_of: ../src/algorithm/geometry/3d/plane_plane_intersection.hpp
---

## API

- `plane_plane_intersection(first, second)`: 2平面の一意な交線を `Line3` で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

平行または一致する平面には `std::domain_error`、零法線には `std::invalid_argument` を送出する。
