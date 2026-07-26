---
title: 3D Segment-Segment Coincidence (三次元線分同士の一致判定)
documentation_of: ../../../../src/algorithm/geometry/3d/segment_segment_coincident.hpp
---

## API

- `coincident(first, second)`: 2本の `Segment3` の端点が順序を除いて一致するなら `true` を返す。

## API別の時間計算量・空間計算量

- `coincident`: 時間・追加領域 $O(1)$。

## 注意点

部分的な重なりは `false`。退化線分同士も、表す点が一致すれば `true` を返す。
