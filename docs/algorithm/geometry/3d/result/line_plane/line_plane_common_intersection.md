---
title: 3D Line-Plane Common Intersection (三次元直線・平面共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/line_plane/line_plane_common_intersection.hpp
---

直線と平面の共通部分を、空集合・1 点・直線のすべての場合について返す。

## API

- `line_plane_common_intersection(line, plane)`: `LinearIntersection3` の `std::monostate`、`Point3`、`Line3` のいずれかを返す。

## API別の時間計算量・空間計算量

- `line_plane_common_intersection`: 時間・追加領域ともに $O(1)$。

## 注意点

- 直線は非退化、平面の法線は非零でなければならず、違反時は `std::invalid_argument` を送出する。非有限な入力にも `std::invalid_argument` を送出する。
- 位置関係の判定と一意な交点の計算は、厳密な二進有理数（dyadic）演算を共有する。
- 真の交点が `long double` で表現できない場合は `std::overflow_error` を送出する。
