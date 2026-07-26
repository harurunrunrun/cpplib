---
title: Distance between 3D Line and Plane in Line-Plane Order (直線・平面順の三次元距離)
documentation_of: ../../../../src/algorithm/geometry/3d/line_plane_distance_line_plane.hpp
---

## API

- `distance(line, plane)`: `Line3` と `Plane3` の集合間距離を返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

交差または重なる場合は0を返す。退化直線・零法線平面・非有限入力には `std::invalid_argument` を送出する。
