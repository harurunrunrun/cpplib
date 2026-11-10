---
title: 3D Segment Type (三次元線分型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/segment3.hpp
---

## API

- `Segment3{a, b}`: 2 端点 `a`, `b` を持つ閉線分を表す集成体型。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域ともに $O(1)$。

## 注意点

`a == b` の場合は 1 点からなる退化した線分を表す。
