---
title: 3D Ray-Segment Parallelism (三次元半直線と線分の平行判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/ray_segment/ray_segment_parallel_ray_segment.hpp
---

## API

- `parallel(first, second)`: 半直線 `first`（`Ray3`）と線分 `second`（`Segment3`）の方向が平行なら `true` を返す。向きが反対でも平行とみなす。

## API別の時間計算量・空間計算量

- `parallel`: 時間・追加領域ともに $O(1)$。

## 注意点

- 同一直線上にあるかどうかは判定しない。方向の平行判定は `GEOMETRY3D_EPS` による相対許容誤差で行う。
- 退化した半直線・線分には `std::invalid_argument` を送出する。
