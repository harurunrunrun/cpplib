---
title: 3D Ray-Plane Common Intersection (三次元半直線・平面共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/ray_plane/ray_plane_common_intersection.hpp
---

半直線と平面の共通部分を、空集合・一点・半直線のいずれの場合も含めて返す。

## API

- `ray_plane_common_intersection(ray, plane)`: `LinearIntersection3` として、交わらなければ `std::monostate`、一点で交われば `Point3`、半直線が平面に含まれれば `Ray3`（`ray` そのもの）を返す。

## API別の時間計算量・空間計算量

- `ray_plane_common_intersection`: 時間・追加空間ともに $O(1)$。

## 注意点

- 半直線は非退化（`origin` と `through` が相異なる）、平面の法線は非零でなければならない。違反した入力や非有限な入力には `std::invalid_argument` を送出する。
- 位置の判定とパラメータ範囲の判定は厳密な 2 進有理数（dyadic）演算を共有する。一意な交点が表現できない場合は `std::overflow_error` を送出する。
