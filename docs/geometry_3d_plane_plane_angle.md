---
title: Angle between 3D Planes (三次元平面同士の角度)
documentation_of: ../src/algorithm/geometry/3d/plane_plane_angle.hpp
---

## API

- `plane_plane_angle(first, second)`: 法線の向きを無視した小さい方の角を $[0,\pi/2]$ で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

零法線には `std::invalid_argument` を送出する。
