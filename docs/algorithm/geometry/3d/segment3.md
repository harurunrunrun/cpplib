---
title: 3D Segment Type (三次元線分型)
documentation_of: ../../../../src/algorithm/geometry/3d/segment3.hpp
---

## API

- `Segment3{a, b}`: 二端点を持つ閉線分を表すaggregate型。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域 $O(1)$。

## 注意点

`a == b` の場合は一点からなる退化線分を表す。
