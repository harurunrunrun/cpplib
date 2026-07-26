---
title: Closest Points from 3D Line to Ray (三次元直線と半直線の順方向最近点対)
documentation_of: ../../../../src/algorithm/geometry/3d/line_ray_closest_points_line_ray.hpp
---

## API

- `closest_points(line, ray)`: `Line3` 上の点と `Ray3` 上の点をこの順のpairで返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

最近点対が複数なら1組を返す。退化直線・退化半直線・非有限入力には `std::invalid_argument`、結果が表現不能なら `std::overflow_error` を送出する。
