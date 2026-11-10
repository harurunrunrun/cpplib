---
title: 3D Ray-Segment Overlap (三次元半直線と線分の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/ray_segment/ray_segment_overlap_ray_segment.hpp
---

## API

- `overlap(first, second)`: 半直線 `first`（`Ray3`）と線分 `second`（`Segment3`）の共通部分が非退化な線分になるとき `true` を返す。

## API別の時間計算量・空間計算量

- `overlap`: 時間・追加領域ともに $O(1)$。

## 注意点

- 1 点だけで交わる場合は `false` を返す。
- 退化した半直線には `std::invalid_argument` を送出する。退化した線分は 1 点として扱うため、結果は `false` になる。
