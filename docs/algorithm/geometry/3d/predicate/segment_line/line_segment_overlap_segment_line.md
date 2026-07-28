---
title: 3D Segment-Line Overlap (三次元線分と直線の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/segment_line/line_segment_overlap_segment_line.hpp
---

## API

- `overlap(first, second)`: `Line3` が `Segment3` 全体を含むとき `true` を返す。

## API別の時間計算量・空間計算量

- `overlap`: 時間・追加領域 $O(1)$。

## 注意点

入力順は線分、直線である。一点だけで交わる場合は `false`。
