---
title: 3D Line-Ray Overlap (三次元直線と半直線の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_ray/line_ray_overlap_line_ray.hpp
---

## API

- `overlap(first, second)`: `Line3` と `Ray3` の共通部分が半直線になるとき `true` を返す。

## API別の時間計算量・空間計算量

- `overlap`: 時間・追加領域 $O(1)$。

## 注意点

一点だけで交わる場合は `false`。入力は各幾何型の有効条件を満たす必要がある。
