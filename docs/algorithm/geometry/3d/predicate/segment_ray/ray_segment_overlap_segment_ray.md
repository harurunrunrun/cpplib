---
title: 3D Segment-Ray Overlap (三次元線分と半直線の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/segment_ray/ray_segment_overlap_segment_ray.hpp
---

## API

- `overlap(first, second)`: `Segment3` と `Ray3` の共通部分が線分になるとき `true` を返す。

## API別の時間計算量・空間計算量

- `overlap`: 時間・追加領域 $O(1)$。

## 注意点

一点だけで交わる場合は `false`。入力順は線分、半直線である。
