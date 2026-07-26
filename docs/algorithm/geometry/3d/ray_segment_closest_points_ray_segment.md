---
title: Closest Points for 3D Ray and Segment in Ray-Segment Order (半直線・線分順の三次元最近点対)
documentation_of: ../../../../src/algorithm/geometry/3d/ray_segment_closest_points_ray_segment.hpp
---

## API

- `closest_points(ray, segment)`: `Ray3` 上の点と `Segment3` 上の点を引数順のpairで返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

線分は退化してもよい。退化半直線・非有限入力には `std::invalid_argument`、結果が表現不能なら `std::overflow_error` を送出する。最近点対が複数なら1組を返す。
