---
title: 3D Ray Intersection (三次元半直線同士の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/ray_ray/ray_ray_intersection.hpp
---

2 本の三次元半直線の共通部分を求める。

## API

- `ray_ray_intersection(first, second)`: 共通部分を `LinearIntersection3` で返す。空集合（`std::monostate`）、点（`Point3`）、線分（`Segment3`）、半直線（`Ray3`）のいずれかである。

## API別の時間計算量・空間計算量

- 時間・追加空間ともに $O(1)$。

## 注意点

- 同一直線上で同じ向きなら、始点が先にある方の半直線を返す。逆向きで重なるなら 2 つの始点を結ぶ線分を、始点どうしでだけ接するならその一点を返す。
- 退化した半直線（`origin` と `through` の距離が `GEOMETRY3D_EPS` 以下）には `std::invalid_argument` を送出する。
