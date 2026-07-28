---
title: 3D Line-Segment Skewness (三次元直線と線分のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_segment/line_segment_skew_line_segment.hpp
---

## API

- `skew(first, second)`: `Line3` と `Segment3` の支持直線がねじれの位置にあるとき `true` を返す。

## API別の時間計算量・空間計算量

- `skew`: 時間・追加領域 $O(1)$。

## 注意点

平行な場合と同一平面上で交差しない場合は `false`。入力は各幾何型の有効条件を満たす必要がある。
