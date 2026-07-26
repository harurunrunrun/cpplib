---
title: Closest Points from 3D Line to Segment (三次元直線と線分の順方向最近点対)
documentation_of: ../../../../src/algorithm/geometry/3d/line_segment_closest_points_line_segment.hpp
---

## API

- `closest_points(line, segment)`: `Line3` 上の点と `Segment3` 上の点をこの順のpairで返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

線分は退化してもよい。退化直線・非有限入力には `std::invalid_argument`、結果が表現不能なら `std::overflow_error` を送出する。
