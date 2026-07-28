---
title: Closest Points from 3D Plane to Line (三次元平面と直線の逆方向最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/plane_line/line_plane_closest_points_plane_line.hpp
---

## API

- `closest_points(plane, line)`: `Plane3` 上の点と `Line3` 上の点をこの順のpairで返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

順方向APIの結果を入れ替えるため、最近点の選択と例外条件は `closest_points(line, plane)` と同じである。
