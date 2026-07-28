---
title: Closest Points from 3D Line to Plane (三次元直線と平面の順方向最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/line_plane/line_plane_closest_points_line_plane.hpp
---

## API

- `closest_points(line, plane)`: `Line3` 上の点と `Plane3` 上の点をこの順のpairで返す。交差時は両要素が交点になる。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

平行時は直線の `a` とその平面射影を返す。退化直線または零法線平面には `std::invalid_argument` を送出する。
