---
title: 3D Triangle Type (三次元三角形型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/triangle3.hpp
---

## API

- `Triangle3{a, b, c}`: 3 頂点 `a`, `b`, `c` を持つ三角形を表す集成体型。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域ともに $O(1)$。

## 注意点

3 頂点が同一直線上にある場合は、退化した三角形を表す。
