---
title: Closest Points from 3D Line to Ray (三次元直線と半直線の順方向最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/line_ray/line_ray_closest_points_line_ray.hpp
---

## API

- `closest_points(line, ray)`: `Line3` 上の点と `Ray3` 上の点を、この順の `std::pair` で返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 最近点対が複数ある場合は、そのうち 1 組を返す。
- 退化直線、退化半直線、非有限入力には `std::invalid_argument` を送出する。結果を表現できない場合は `std::overflow_error` を送出する。
