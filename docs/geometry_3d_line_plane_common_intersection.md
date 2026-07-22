---
title: 3D Line-Plane Common Intersection (三次元直線・平面共通部分)
documentation_of: ../src/algorithm/geometry/3d/query/line_plane_common_intersection.hpp
---

## 概要

直線と平面の共通部分を、空集合・一点・直線のすべてについて返す。

## API

- `line_plane_common_intersection(line, plane)`: `LinearIntersection3` の `std::monostate`、`Point3`、`Line3` のいずれかを返す。

## API別の時間計算量・空間計算量

- `line_plane_common_intersection`: 時間・追加領域ともに $O(1)$。

## 注意点

直線は非退化、平面法線は非零でなければならない。非有限入力には `std::invalid_argument` を送出する。
位置判定と一意な交点はexact dyadic演算を共有する。真の交点が `long double` で
表現できない場合は `std::overflow_error` を送出する。
