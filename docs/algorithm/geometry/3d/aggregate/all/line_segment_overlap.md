---
title: 3D Line-Segment Overlap (三次元直線と線分の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/line_segment_overlap.hpp
---

## API

- `overlap(line, segment)`: `Line3` と `Segment3` の共通部分が正の長さを持つ（線分が直線上にある）なら `true` を返す。
- `overlap(segment, line)`: 引数の順序を入れ替えた同じ判定を行う。

## API 別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 退化した線分や、1 点だけで接する場合は `false` を返す。
- 退化した直線には `std::invalid_argument` を送出する。
