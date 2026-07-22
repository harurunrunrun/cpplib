---
title: 3D Collinearity (三次元共線判定)
documentation_of: ../src/algorithm/geometry/3d/predicate/collinear.hpp
---

## API

- `collinear(first, second, third)`: 3点が同一直線上にあるか判定する。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

一致する点を含む退化配置も共線と判定する。
