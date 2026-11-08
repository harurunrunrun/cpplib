---
title: Distance between 3D Line and Plane in Line-Plane Order (直線・平面順の三次元距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/line_plane/line_plane_distance_line_plane.hpp
---

## API

- `distance(line, plane)`: `Line3` と `Plane3` の集合間距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 交差する、または直線が平面上にある場合は $0$ を返す。平行な場合は直線上の点 `line.a` と平面の距離になる。
- 退化した直線、法線が零ベクトルの平面、非有限な入力には `std::invalid_argument` を送出する。
