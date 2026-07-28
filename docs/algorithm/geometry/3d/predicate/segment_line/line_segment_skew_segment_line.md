---
title: 3D Segment-Line Skewness (三次元線分と直線のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/segment_line/line_segment_skew_segment_line.hpp
---

## API

- `skew(first, second)`: `Segment3` と `Line3` の支持直線がねじれの位置にあるとき `true` を返す。

## API別の時間計算量・空間計算量

- `skew`: 時間・追加領域 $O(1)$。

## 注意点

平行な場合と同一平面上で交差しない場合は `false`。入力順は線分、直線である。
