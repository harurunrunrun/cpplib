---
title: Plane-Point Reflection (平面に関する点の鏡映)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/plane_point/plane_point_reflection.hpp
---

## API

- `reflection(plane, point)`: 点を平面に関して鏡映した点を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

平面の法線は零ベクトルでなく、入力座標は有限値でなければならない。違反時は `std::invalid_argument` を送出する。
