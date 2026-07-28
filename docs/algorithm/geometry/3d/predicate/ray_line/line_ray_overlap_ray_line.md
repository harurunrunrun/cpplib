---
title: 3D Ray-Line Overlap (三次元半直線と直線の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/ray_line/line_ray_overlap_ray_line.hpp
---

## API

- `overlap(first, second)`: `Ray3` と `Line3` の共通部分が半直線になるとき `true` を返す。

## API別の時間計算量・空間計算量

- `overlap`: 時間・追加領域 $O(1)$。

## 注意点

一点だけで交わる場合は `false`。入力順は半直線、直線である。
