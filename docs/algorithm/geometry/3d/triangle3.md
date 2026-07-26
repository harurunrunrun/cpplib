---
title: 3D Triangle Type (三次元三角形型)
documentation_of: ../../../../src/algorithm/geometry/3d/triangle3.hpp
---

## API

- `Triangle3{a, b, c}`: 三頂点を持つ三角形を表すaggregate型。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域 $O(1)$。

## 注意点

三頂点が同一直線上の場合は退化三角形を表す。
