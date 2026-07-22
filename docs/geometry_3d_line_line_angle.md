---
title: Angle between 3D Lines (三次元直線同士の角度)
documentation_of: ../src/algorithm/geometry/3d/scalar/line_line_angle.hpp
---

## API

- `line_line_angle(first, second)`: 向きを無視した小さい方の角を $[0,\pi/2]$ で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化直線には `std::invalid_argument` を送出する。
