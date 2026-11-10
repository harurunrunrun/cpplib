---
title: 3D Ray-Plane Intersection (三次元半直線と平面の交点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/ray_plane/ray_plane_intersection.hpp
---

半直線と平面の一意な交点を求める。

## API

- `ray_plane_intersection(ray, plane)`: 半直線上の一意な交点を `std::optional<Point3>` で返す。交点が存在しない場合や、半直線が平面に含まれて交点が無数にある場合は `std::nullopt` を返す。

## API別の時間計算量・空間計算量

- 時間・追加空間ともに $O(1)$。

## 注意点

- 退化した半直線（`origin` と `through` が一致）、零法線、非有限な入力には `std::invalid_argument` を送出する。
- 始点上の交点も含む。
- 交点が存在する範囲は厳密な 2 進有理数（dyadic）によるパラメータの符号で判定する。交点が表現できない場合は `std::overflow_error` を送出する。
