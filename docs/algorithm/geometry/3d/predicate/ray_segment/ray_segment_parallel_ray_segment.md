---
title: 3D Ray-Segment Parallelism (三次元半直線と線分の平行判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/ray_segment/ray_segment_parallel_ray_segment.hpp
---

## API

- `parallel(first, second)`: `Ray3` と `Segment3` の方向が平行なら `true` を返す。

## API別の時間計算量・空間計算量

- `parallel`: 時間・追加領域 $O(1)$。

## 注意点

同一直線上にあるかや向きが一致するかは判定しない。入力は各幾何型の有効条件を満たす必要がある。
