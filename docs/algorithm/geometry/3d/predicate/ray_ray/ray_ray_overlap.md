---
title: 3D Ray-Ray Overlap (三次元半直線同士の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/ray_ray/ray_ray_overlap.hpp
---

## API

- `overlap(first, second)`: 2 本の `Ray3` の共通部分が正の長さを持つ、すなわち半直線または非退化な線分になるなら `true` を返す。

## API別の時間計算量・空間計算量

- `overlap`: 時間・追加領域ともに $O(1)$。

## 注意点

- 始点の 1 点だけを共有する場合は `false` を返す。
- いずれかが退化した半直線なら `std::invalid_argument` を送出する。
