---
title: 3D Ray-Ray Overlap (三次元半直線同士の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/ray_ray/ray_ray_overlap.hpp
---

## API

- `overlap(first, second)`: 2本の `Ray3` の共通部分が正の長さを持つなら `true` を返す。共通部分が半直線または非退化線分のときに該当する。

## API別の時間計算量・空間計算量

- `overlap`: 時間・追加領域 $O(1)$。

## 注意点

始点1点だけを共有する場合は `false`。いずれかが退化半直線なら `std::invalid_argument` を送出する。
