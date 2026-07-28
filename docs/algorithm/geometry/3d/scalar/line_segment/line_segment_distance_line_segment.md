---
title: Distance between 3D Line and Segment in Line-Segment Order (直線・線分順の三次元距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/line_segment/line_segment_distance_line_segment.hpp
---

## API

- `distance(line, segment)`: `Line3` と `Segment3` の集合間距離を返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

交差または重なる場合は0を返す。線分は退化してもよい。退化直線・非有限入力には `std::invalid_argument`、距離が表現不能なら `std::overflow_error` を送出する。
