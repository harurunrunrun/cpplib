---
title: 3D Line-Plane Intersection (三次元直線と平面の交点)
documentation_of: ../src/algorithm/geometry/3d/line_plane_intersection.hpp
---

## API

- `line_plane_intersection(line, plane)`: 無限直線と平面の一意な交点を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

平行または直線全体が平面上なら `std::domain_error`、退化直線・零法線には `std::invalid_argument` を送出する。
