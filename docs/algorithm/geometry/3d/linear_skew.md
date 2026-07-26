---
title: 3D Linear Skewness Umbrella (三次元線形図形のねじれ判定集約)
documentation_of: ../../../../src/algorithm/geometry/3d/linear_skew.hpp
---

## API

- `skew(first, second)`: `Line3`, `Ray3`, `Segment3` の支持直線同士が非平行かつ交わらないかを返す。
- このヘッダは型ペア別の `*_skew.hpp` をまとめてincludeする互換用集約ヘッダである。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

必要な型ペアだけを使う場合は対応するleafヘッダを直接includeできる。有限範囲が交わらないだけではねじれとしない。
