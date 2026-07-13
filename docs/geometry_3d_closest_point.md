---
title: Closest Point on 3D Primitive (三次元図形上の最近点)
documentation_of: ../src/algorithm/geometry/3d/closest_point.hpp
---

## API

- `closest_point(ray, point)`: 半直線上の最近点。
- `closest_point(segment, point)`: 閉線分上の最近点。
- `closest_point(triangle, point)`: 閉三角形上の最近点。

## API別の時間計算量・空間計算量

- 各overload: 時間・追加領域 $O(1)$。

## 注意点

退化線分には対応する。退化半直線または退化三角形には `std::invalid_argument` を送出する。
