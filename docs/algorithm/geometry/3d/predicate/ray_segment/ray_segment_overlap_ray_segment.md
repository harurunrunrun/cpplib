---
title: 3D Ray-Segment Overlap (三次元半直線と線分の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/ray_segment/ray_segment_overlap_ray_segment.hpp
---

## API

- `overlap(first, second)`: `Ray3` と `Segment3` の共通部分が線分になるとき `true` を返す。

## API別の時間計算量・空間計算量

- `overlap`: 時間・追加領域 $O(1)$。

## 注意点

一点だけで交わる場合は `false`。入力は各幾何型の有効条件を満たす必要がある。
