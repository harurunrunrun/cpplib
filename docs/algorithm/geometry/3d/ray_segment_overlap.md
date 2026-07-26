---
title: 3D Ray-Segment Overlap (三次元半直線と線分の重なり判定)
documentation_of: ../../../../src/algorithm/geometry/3d/ray_segment_overlap.hpp
---

## API

- `overlap(ray, segment)`: `Ray3` と `Segment3` の共通部分が正の長さを持つなら `true` を返す。
- `overlap(segment, ray)`: 引数順を逆にした同じ判定を行う。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

退化線分や1点だけの接触には `false` を返す。退化半直線なら `std::invalid_argument` を送出する。
