---
title: 3D Segment-Plane Common Intersection (三次元線分・平面共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/segment_plane/segment_plane_common_intersection.hpp
---

線分と平面の共通部分を、空集合・一点・線分のいずれの場合も含めて返す。

## API

- `segment_plane_common_intersection(segment, plane)`: `LinearIntersection3` として、交わらなければ `std::monostate`、一点で交われば `Point3`、線分が平面に含まれれば `Segment3`（`segment` そのもの）を返す。両端点が一致する退化線分が平面上にある場合は、その一点を返す。

## API別の時間計算量・空間計算量

- `segment_plane_common_intersection`: 時間・追加空間ともに $O(1)$。

## 注意点

- 平面の法線は非零でなければならない。零法線や非有限な入力には `std::invalid_argument` を送出する。
- 位置の判定と閉区間に入るかの判定は厳密な 2 進有理数（dyadic）演算を共有する。一意な交点が表現できない場合は `std::overflow_error` を送出する。
