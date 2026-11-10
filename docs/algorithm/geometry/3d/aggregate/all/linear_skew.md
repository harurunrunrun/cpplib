---
title: 3D Linear Skewness Umbrella (三次元線形図形のねじれ判定集約)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/linear_skew.hpp
---

## API

- `skew(first, second)`: `Line3`, `Ray3`, `Segment3` のすべての組合せについて、支持直線同士が平行でなく、かつ交わらない（ねじれの位置にある）かを返す。

このヘッダは型の組ごとの `*_skew.hpp` をまとめて include する、互換用の集約ヘッダである。必要な型の組だけを使う場合は、対応する個別ヘッダを直接 include すればよい。

## API 別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 有限な範囲同士が交わらないだけでは、ねじれの位置とはみなさない。
