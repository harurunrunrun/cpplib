---
title: 3D Segment-Line Skewness (三次元線分と直線のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/segment_line/line_segment_skew_segment_line.hpp
---

## API

- `skew(first, second)`: 線分 `first`（`Segment3`）の支持直線と直線 `second`（`Line3`）がねじれの位置にあるとき `true` を返す。

## API別の時間計算量・空間計算量

- `skew`: 時間・追加領域ともに $O(1)$。

## 注意点

- 線分の範囲は考慮しない。平行な場合と、支持直線同士が交わる場合は `false` を返す。
- 引数の順序は線分、直線である。
- 退化した線分・直線には `std::invalid_argument` を送出する。
