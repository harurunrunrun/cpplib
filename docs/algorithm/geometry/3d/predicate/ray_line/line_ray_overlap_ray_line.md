---
title: 3D Ray-Line Overlap (三次元半直線と直線の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/ray_line/line_ray_overlap_ray_line.hpp
---

## API

- `overlap(first, second)`: 半直線 `first`（`Ray3`）と直線 `second`（`Line3`）の共通部分が半直線になる、すなわち半直線が直線上にあるとき `true` を返す。

## API別の時間計算量・空間計算量

- `overlap`: 時間・追加領域ともに $O(1)$。

## 注意点

- 1 点だけで交わる場合や交わらない場合は `false` を返す。
- 引数の順序は半直線、直線である。実装は `overlap(second, first)` に委譲する。
- 退化した半直線・直線には `std::invalid_argument` を送出する。
