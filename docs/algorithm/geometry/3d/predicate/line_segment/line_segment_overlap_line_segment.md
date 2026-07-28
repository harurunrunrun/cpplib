---
title: 3D Line-Segment Overlap (三次元直線と線分の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_segment/line_segment_overlap_line_segment.hpp
---

## API

- `overlap(first, second)`: `Line3` が `Segment3` 全体を含むとき `true` を返す。

## API別の時間計算量・空間計算量

- `overlap`: 時間・追加領域 $O(1)$。

## 注意点

一点だけで交わる場合は `false`。入力は各幾何型の有効条件を満たす必要がある。
