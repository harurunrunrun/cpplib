---
title: 3D Segment-Line Parallelism (三次元線分と直線の平行判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/segment_line/line_segment_parallel_segment_line.hpp
---

## API

- `parallel(first, second)`: `Segment3` と `Line3` の方向が平行なら `true` を返す。

## API別の時間計算量・空間計算量

- `parallel`: 時間・追加領域 $O(1)$。

## 注意点

同一直線上にあるかや向きが一致するかは判定しない。入力順は線分、直線である。
