---
title: 3D Tetrahedron Type (三次元四面体型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/tetrahedron3.hpp
---

## API

- `Tetrahedron3{a, b, c, d}`: 4 頂点 `a`, `b`, `c`, `d` を持つ四面体を表す集成体型。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域ともに $O(1)$。

## 注意点

4 頂点が同一平面上にある場合は、退化した四面体を表す。
