---
title: 3D Tetrahedron Type (三次元四面体型)
documentation_of: ../../../../src/algorithm/geometry/3d/tetrahedron3.hpp
---

## API

- `Tetrahedron3{a, b, c, d}`: 四頂点を持つ四面体を表すaggregate型。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域 $O(1)$。

## 注意点

四頂点が同一平面上の場合は退化四面体を表す。
