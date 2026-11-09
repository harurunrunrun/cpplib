---
title: 3D Linear Overlap Umbrella (三次元線形図形の重なり判定集約)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/linear_overlap.hpp
---

## API

- `overlap(first, second)`: `Line3`, `Ray3`, `Segment3` のすべての組合せについて、共通部分が正の長さを持つかを返す。

このヘッダは型の組ごとの `*_overlap.hpp` をまとめて include する、互換用の集約ヘッダである。必要な型の組だけを使う場合は、対応する個別ヘッダを直接 include すればよい。

## API 別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 1 点だけで接する場合は `false` を返す。
