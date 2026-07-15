---
title: 3D Ray-Plane Common Intersection (三次元半直線・平面共通部分)
documentation_of: ../src/algorithm/geometry/3d/ray_plane_common_intersection.hpp
---

## 概要

半直線と平面の共通部分を、空集合・一点・半直線のすべてについて返す。

## API

- `ray_plane_common_intersection(ray, plane)`: `LinearIntersection3` の `std::monostate`、`Point3`、`Ray3` のいずれかを返す。

## API別の時間計算量・空間計算量

- `ray_plane_common_intersection`: 時間・追加領域ともに $O(1)$。

## 注意点

半直線は非退化、平面法線は非零でなければならない。非有限入力には `std::invalid_argument` を送出する。
