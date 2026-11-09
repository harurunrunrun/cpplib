---
title: Closest Points for 3D Ray and Plane in Ray-Plane Order (半直線・平面順の三次元最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/ray_plane/ray_plane_closest_points_ray_plane.hpp
---

## API

- `closest_points(ray, plane)`: `Ray3` 上の点と `Plane3` 上の点を、引数と同じ順の `std::pair<Point3, Point3>` で返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 半直線と平面が交わるときは交点を 2 つ並べた組を、交わらないときは半直線の始点とその平面への射影の組を返す。
- 退化した半直線、零法線の平面、非有限な入力には `std::invalid_argument` を送出する。
