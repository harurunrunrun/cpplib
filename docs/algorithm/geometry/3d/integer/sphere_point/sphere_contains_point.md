---
title: Sphere Contains Point (球の点包含判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/integer/sphere_point/sphere_contains_point.hpp
---

## API

- `contains(sphere, point)`: 点が球の内部なら `1`、球面上なら `0`、外部なら `-1` を返す。

## API別の時間計算量・空間計算量

- 全 API: 時間・追加領域ともに $O(1)$。

## 注意点

中心・半径・点は有限で、半径は非負でなければならない。違反時は `std::invalid_argument` を送出する。
球面上かどうかは、中心からの距離と半径の大きい方をスケールとする相対許容誤差 `GEOMETRY3D_EPS` で判定する。
