---
title: Plane-Point Projection (平面への点の射影)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/plane_point/plane_point_projection.hpp
---

## API

- `projection(plane, point)`: `point` から平面へ下ろした垂線の足を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 平面の法線は零ベクトルであってはならず、全座標は有限値でなければならない。違反時は `std::invalid_argument` を送出する。
- `point` が平面上にあると厳密に判定された場合は `point` 自身を返す。
