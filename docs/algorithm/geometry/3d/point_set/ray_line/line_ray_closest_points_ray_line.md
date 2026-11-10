---
title: Closest Points from 3D Ray to Line (三次元半直線と直線の逆方向最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/ray_line/line_ray_closest_points_ray_line.hpp
---

## API

- `closest_points(ray, line)`: `Ray3` 上の点と `Line3` 上の点を、引数と同じ順の `std::pair<Point3, Point3>` で返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

`closest_points(line, ray)` の結果を入れ替えて返すだけなので、最近点の選び方と例外条件は `closest_points(line, ray)` と同じである。
