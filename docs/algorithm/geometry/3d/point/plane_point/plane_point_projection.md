---
title: Plane-Point Projection (平面への点の射影)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/plane_point/plane_point_projection.hpp
---

## API

- `projection(plane, point)`: 点から平面へ下ろした垂線の足を返す。

## API別の時間計算量・空間計算量

- 全API: 時間・追加領域 $O(1)$。

## 注意点

平面法線は零ベクトルであってはならず、全座標は有限値でなければならない。
