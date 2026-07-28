---
title: 3D Ray Type (三次元半直線型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/ray3.hpp
---

## API

- `Ray3{origin, through}`: 始点と通過点で半直線を表すaggregate型。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域 $O(1)$。

## 注意点

`origin == through` の退化半直線を許容するかは利用する個別APIに従う。
