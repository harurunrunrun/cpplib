---
title: 3D Segment-Line Parallelism (三次元線分と直線の平行判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/segment_line/line_segment_parallel_segment_line.hpp
---

## API

- `parallel(first, second)`: 線分 `first`（`Segment3`）と直線 `second`（`Line3`）の方向が平行なら `true` を返す。向きが反対でも平行とみなす。

## API別の時間計算量・空間計算量

- `parallel`: 時間・追加領域ともに $O(1)$。

## 注意点

- 同一直線上にあるかどうかは判定しない。方向の平行判定は `GEOMETRY3D_EPS` による相対許容誤差で行う。
- 引数の順序は線分、直線である。
- 退化した線分・直線には `std::invalid_argument` を送出する。
