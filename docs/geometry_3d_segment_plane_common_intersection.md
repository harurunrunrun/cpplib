---
title: 3D Segment-Plane Common Intersection (三次元線分・平面共通部分)
documentation_of: ../src/algorithm/geometry/3d/query/segment_plane_common_intersection.hpp
---

## 概要

線分と平面の共通部分を、空集合・一点・線分のすべてについて返す。

## API

- `segment_plane_common_intersection(segment, plane)`: `LinearIntersection3` の `std::monostate`、`Point3`、`Segment3` のいずれかを返す。退化線分が平面上にある場合は一点を返す。

## API別の時間計算量・空間計算量

- `segment_plane_common_intersection`: 時間・追加領域ともに $O(1)$。

## 注意点

平面法線は非零でなければならない。非有限入力には `std::invalid_argument` を送出する。
位置判定と閉区間判定はexact dyadic演算を共有し、表現不能な一意交点には
`std::overflow_error` を送出する。
