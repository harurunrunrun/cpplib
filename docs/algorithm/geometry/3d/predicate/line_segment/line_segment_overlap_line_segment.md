---
title: 3D Line-Segment Overlap (三次元直線と線分の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_segment/line_segment_overlap_line_segment.hpp
---

## API

- `overlap(first, second)`: `Line3` が `Segment3` 全体を含む（共通部分が線分になる）とき `true` を返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 1 点だけで交わる場合は `false` を返す。
- 入力は各幾何型の有効条件を満たさなければならない。
