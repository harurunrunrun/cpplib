---
title: 3D Linear Coincidence Umbrella (三次元線形図形の一致判定集約)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/linear_coincident.hpp
---

## API

- `coincident(Line3, Line3)`: 無限直線が点集合として一致するかを返す。
- `coincident(Ray3, Ray3)`: 始点と向きを含めて半直線が一致するかを返す。
- `coincident(Segment3, Segment3)`: 端点順を無視して閉線分が一致するかを返す。
- このヘッダは同じ型同士の `*_coincident.hpp` をまとめてincludeする互換用集約ヘッダである。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

必要な型だけを使う場合は対応するleafヘッダを直接includeできる。退化直線・半直線には `std::invalid_argument` を送出する。
