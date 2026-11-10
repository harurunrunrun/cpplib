---
title: 3D Segment-Line Overlap (三次元線分と直線の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/segment_line/line_segment_overlap_segment_line.hpp
---

## API

- `overlap(first, second)`: 直線 `second`（`Line3`）が線分 `first`（`Segment3`）全体を含む、すなわち共通部分が非退化な線分になるとき `true` を返す。

## API別の時間計算量・空間計算量

- `overlap`: 時間・追加領域ともに $O(1)$。

## 注意点

- 引数の順序は線分、直線である。実装は `overlap(second, first)` に委譲する。
- 1 点だけで交わる場合は `false` を返す。退化した線分は 1 点として扱うため、結果は `false` になる。
- 退化した直線には `std::invalid_argument` を送出する。
