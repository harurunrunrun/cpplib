---
title: 3D Line-Ray Overlap (三次元直線と半直線の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_ray/line_ray_overlap_line_ray.hpp
---

## API

- `overlap(first, second)`: `Line3` と `Ray3` の共通部分が半直線になる（半直線全体が直線に含まれる）とき `true` を返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 1 点だけで交わる場合は `false` を返す。
- 入力は各幾何型の有効条件（退化していない、有限である等）を満たさなければならない。
