---
title: Closest Point on 3D Sphere (三次元球面上の最近点)
documentation_of: ../src/algorithm/geometry/3d/sphere_closest_point.hpp
---

## API

- `closest_point(sphere, point)`: 球面上で点に最も近い点を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

負半径には `std::invalid_argument`、正半径の球の中心では `std::domain_error` を送出する。半径0では中心を返す。
