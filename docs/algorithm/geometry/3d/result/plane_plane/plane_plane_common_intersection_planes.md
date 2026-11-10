---
title: Common Intersection of Two Planes (2平面の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/plane_plane/plane_plane_common_intersection_planes.hpp
---

2 個の三次元平面の共通部分を、空集合・直線・平面のいずれかとして返す。

## API

- `plane_plane_common_intersection(first, second)`: `PlanePlaneIntersection3` を返す。2 平面が平行で交わらなければ `std::monostate`、1 直線で交われば `Line3`、一致すれば `first` を `Plane3` として保持する。

## API別の時間計算量・空間計算量

時間・追加空間ともに $O(1)$。

## 注意点

両平面の点と法線は有限値で、法線は非零でなければならない。非有限な入力や零法線には `std::invalid_argument` を送出する。
