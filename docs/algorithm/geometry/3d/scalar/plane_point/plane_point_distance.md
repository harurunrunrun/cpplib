---
title: Plane-Point Distance (平面と点の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/plane_point/plane_point_distance.hpp
---

## API

- `distance(plane, point)`: 平面と点の符号なし距離を返す。

## API別の時間計算量・空間計算量

- `distance`: 時間・追加領域 $O(1)$。

## 注意点

平面法線は零ベクトルであってはならない。
