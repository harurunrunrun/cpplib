---
title: 3D Segment-Segment Overlap (三次元線分同士の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/segment_segment/segment_segment_overlap.hpp
---

## API

- `overlap(first, second)`: 2 本の `Segment3` の共通部分が正の長さを持つ（非退化な線分になる）なら `true` を返す。

## API別の時間計算量・空間計算量

- `overlap`: 時間・追加領域ともに $O(1)$。

## 注意点

- 端点 1 点だけで接する場合や、いずれかが退化した線分の場合は `false` を返す。
