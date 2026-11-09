---
title: 3D Segment-Plane Intersection (三次元線分と平面の交点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/segment_plane/segment_plane_intersection.hpp
---

線分と平面の一意な交点を求める。

## API

- `segment_plane_intersection(segment, plane)`: 閉線分上の一意な交点を `std::optional<Point3>` で返す。交点が存在しない場合や、非退化の線分全体が平面上にある場合は `std::nullopt` を返す。

## API別の時間計算量・空間計算量

- 時間・追加空間ともに $O(1)$。

## 注意点

- 両端点が一致する退化線分が平面上にある場合は、その点を返す。
- 零法線や非有限な入力には `std::invalid_argument` を送出する。
- 閉区間に入るかどうかは、両端点についての厳密な 2 進有理数（dyadic）の符号で判定する。一意な交点が表現できない場合は `std::overflow_error` を送出する。
