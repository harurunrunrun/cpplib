---
title: 3D Plane-Plane Common Intersection (三次元平面同士の共通部分)
documentation_of: ../src/algorithm/geometry/3d/query/plane_plane_common_intersection.hpp
---

## 概要

二平面の共通部分を、空集合・直線・平面のすべてについて返す。

## API

- `PlanePlaneIntersection3`: `std::variant<std::monostate, Line3, Plane3>`。
- `plane_plane_common_intersection(first, second)`: 二平面の共通部分を返す。

## API別の時間計算量・空間計算量

- 型エイリアスの利用: 時間・追加領域ともに $O(1)$。
- `plane_plane_common_intersection`: 時間・追加領域ともに $O(1)$。

## 注意点

両平面の法線は非零でなければならない。非有限入力には `std::invalid_argument` を送出する。
