---
title: Closest Points for 3D Segment and Plane in Segment-Plane Order (線分・平面順の三次元最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/segment_plane/segment_plane_closest_points_segment_plane.hpp
---

## API

- `closest_points(segment, plane)`: `Segment3` 上の点と `Plane3` 上の点を、引数と同じ順の `std::pair<Point3, Point3>` で返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 線分は退化（両端点が一致）していてもよい。
- 線分と平面が交わるときは交点を 2 つ並べた組を、交わらないときは平面までの距離が小さい方の端点とその平面への射影の組を返す。
- 零法線の平面、非有限な入力には `std::invalid_argument` を送出する。
