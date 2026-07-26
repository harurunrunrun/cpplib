---
title: 3D Line-Line Overlap (三次元直線同士の重なり判定)
documentation_of: ../../../../src/algorithm/geometry/3d/line_line_overlap.hpp
---

## API

- `overlap(first, second)`: 2本の `Line3` の共通部分が正の長さを持つなら `true` を返す。直線同士では集合として一致するときに限る。

## API別の時間計算量・空間計算量

- `overlap`: 時間・追加領域 $O(1)$。

## 注意点

1点での交差は `false`。いずれかが退化直線なら `std::invalid_argument` を送出する。
