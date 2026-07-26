---
title: 3D Segment-Segment Overlap (三次元線分同士の重なり判定)
documentation_of: ../../../../src/algorithm/geometry/3d/segment_segment_overlap.hpp
---

## API

- `overlap(first, second)`: 2本の `Segment3` の共通部分が正の長さを持つなら `true` を返す。

## API別の時間計算量・空間計算量

- `overlap`: 時間・追加領域 $O(1)$。

## 注意点

端点1点だけの接触や退化線分には `false` を返す。
