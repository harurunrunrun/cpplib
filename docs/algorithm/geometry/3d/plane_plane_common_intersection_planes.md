---
title: Common Intersection of Two Planes (2平面の共通部分)
documentation_of: ../../../../src/algorithm/geometry/3d/plane_plane_common_intersection_planes.hpp
---

2個の3次元平面の共通部分を、空集合・直線・平面のいずれかとして返します。

## API

- `plane_plane_common_intersection(first, second)`: 交わらなければ `std::monostate`、1直線で交われば `Line3`、一致すれば `Plane3` を保持する結果を返します。

## API別の時間計算量・空間計算量

時間・追加空間ともに $O(1)$ です。

## 注意点

両平面の点と法線は有限値で、法線は非零でなければなりません。非有限入力には `std::invalid_argument` を送出します。