---
title: 3D Ray-Line Parallelism (三次元半直線と直線の平行判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/ray_line/line_ray_parallel_ray_line.hpp
---

## API

- `parallel(first, second)`: `Ray3` と `Line3` の方向が平行なら `true` を返す。

## API別の時間計算量・空間計算量

- `parallel`: 時間・追加領域 $O(1)$。

## 注意点

同一直線上にあるかや向きが一致するかは判定しない。入力順は半直線、直線である。
