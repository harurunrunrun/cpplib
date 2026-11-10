---
title: 3D Line-Line Overlap (三次元直線同士の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_line/line_line_overlap.hpp
---

## API

- `overlap(first, second)`: 2 本の `Line3` の共通部分が正の長さを持つなら `true` を返す。直線同士では、集合として一致するときに限る。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 1 点で交わる場合は `false` を返す。
- いずれかが退化した直線なら `std::invalid_argument` を送出する。
