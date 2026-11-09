---
title: 3D Linear Parallelism Umbrella (三次元線形図形の平行判定集約)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/linear_parallel.hpp
---

## API

- `parallel(first, second)`: `Line3`, `Ray3`, `Segment3` のすべての順序付きの組合せについて、方向が平行かを返す。

このヘッダは型の組ごとの `*_parallel.hpp` をまとめて include する、互換用の集約ヘッダである。必要な型の組だけを使う場合は、対応する個別ヘッダを直接 include すればよい。

## API 別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 退化して方向を持たない図形には `std::invalid_argument` を送出する。
