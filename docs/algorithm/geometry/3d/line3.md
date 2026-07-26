---
title: 3D Line Type (三次元直線型)
documentation_of: ../../../../src/algorithm/geometry/3d/line3.hpp
---

## API

- `Line3{a, b}`: 相異なる二点を通る直線を表すaggregate型。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域 $O(1)$。

## 注意点

`a == b` の退化直線を許容するかは利用する個別APIに従う。
