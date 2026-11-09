---
title: 3D Segment-Ray Overlap (三次元線分と半直線の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/segment_ray/ray_segment_overlap_segment_ray.hpp
---

## API

- `overlap(first, second)`: 線分 `first`（`Segment3`）と半直線 `second`（`Ray3`）の共通部分が非退化な線分になるとき `true` を返す。

## API別の時間計算量・空間計算量

- `overlap`: 時間・追加領域ともに $O(1)$。

## 注意点

- 1 点だけで交わる場合は `false` を返す。
- 引数の順序は線分、半直線である。実装は `overlap(second, first)` に委譲する。
- 退化した半直線には `std::invalid_argument` を送出する。退化した線分は 1 点として扱うため、結果は `false` になる。
