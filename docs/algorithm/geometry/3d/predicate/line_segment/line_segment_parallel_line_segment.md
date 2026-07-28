---
title: 3D Line-Segment Parallelism (三次元直線と線分の平行判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_segment/line_segment_parallel_line_segment.hpp
---

## API

- `parallel(first, second)`: `Line3` と `Segment3` の方向が平行なら `true` を返す。

## API別の時間計算量・空間計算量

- `parallel`: 時間・追加領域 $O(1)$。

## 注意点

同一直線上にあるかや向きが一致するかは判定しない。入力は各幾何型の有効条件を満たす必要がある。
