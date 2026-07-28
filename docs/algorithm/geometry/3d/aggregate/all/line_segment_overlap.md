---
title: 3D Line-Segment Overlap (三次元直線と線分の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/line_segment_overlap.hpp
---

## API

- `overlap(line, segment)`: `Line3` と `Segment3` の共通部分が正の長さを持つなら `true` を返す。
- `overlap(segment, line)`: 引数順を逆にした同じ判定を行う。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

退化線分や1点だけの接触には `false` を返す。退化直線なら `std::invalid_argument` を送出する。
