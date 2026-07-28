---
title: 3D Linear Parallelism Umbrella (三次元線形図形の平行判定集約)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/linear_parallel.hpp
---

## API

- `parallel(first, second)`: `Line3`, `Ray3`, `Segment3` の全順序付き組合せについて方向が平行かを返す。
- このヘッダは型ペア別の `*_parallel.hpp` をまとめてincludeする互換用集約ヘッダである。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

必要な型ペアだけを使う場合は対応するleafヘッダを直接includeできる。退化して方向を持たない図形には `std::invalid_argument` を送出する。
