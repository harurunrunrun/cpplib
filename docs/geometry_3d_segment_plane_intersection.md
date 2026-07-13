---
title: 3D Segment-Plane Intersection (三次元線分と平面の交点)
documentation_of: ../src/algorithm/geometry/3d/segment_plane_intersection.hpp
---

## API

- `segment_plane_intersection(segment, plane)`: 閉線分上の一意な交点を返し、存在しないか非退化線分全体が平面上なら `nullopt`。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

平面上の退化線分はその点を返す。零法線には `std::invalid_argument` を送出する。
