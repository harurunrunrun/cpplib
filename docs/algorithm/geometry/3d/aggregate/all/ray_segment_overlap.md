---
title: 3D Ray-Segment Overlap (三次元半直線と線分の重なり判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/ray_segment_overlap.hpp
---

## API

- `overlap(ray, segment)`: `Ray3` と `Segment3` の共通部分が正の長さを持つ（共通部分が線分になる）なら `true` を返す。
- `overlap(segment, ray)`: 引数の順を逆にした同じ判定を行う。

## API別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加領域ともに $O(1)$。

## 注意点

退化した線分や、1 点だけで接する場合には `false` を返す。退化した半直線には `std::invalid_argument` を送出する。
