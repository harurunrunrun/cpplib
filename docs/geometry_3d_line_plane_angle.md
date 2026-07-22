---
title: Angle between 3D Line and Plane (三次元直線と平面の角度)
documentation_of: ../src/algorithm/geometry/3d/scalar/line_plane_angle.hpp
---

## API

- `line_plane_angle(line, plane)`: 直線と平面の小さい方の角を $[0,\pi/2]$ で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

平行なら0、垂直なら $\pi/2$。退化直線または零法線には `std::invalid_argument` を送出する。
