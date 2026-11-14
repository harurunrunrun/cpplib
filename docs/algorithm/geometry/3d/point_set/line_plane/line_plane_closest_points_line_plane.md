---
title: Closest Points from 3D Line to Plane (三次元直線と平面の順方向最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/line_plane/line_plane_closest_points_line_plane.hpp
---

## API

- `closest_points(line, plane)`: `Line3` 上の点と `Plane3` 上の点を、この順の `std::pair` で返す。交差する場合は両要素とも交点になる。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 平行判定は、方向ベクトル `line.b - line.a` と法線の内積の符号を厳密に求めて行う（`line_plane_intersection` と同じ判定）。厳密に平行な場合は、直線の `a` とその平面への射影を返す。
- 平行でない場合は交点を厳密に求めて丸める。わずかに傾いているだけの直線でも、遠方の交点を返す。交点を表現できない場合は `std::overflow_error` を送出する。
- 方向ベクトルの長さが `GEOMETRY3D_EPS` 以下の退化直線、零法線の平面、非有限入力には `std::invalid_argument` を送出する。
