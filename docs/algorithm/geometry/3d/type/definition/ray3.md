---
title: 3D Ray Type (三次元半直線型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/ray3.hpp
---

## API

- `Ray3{origin, through}`: 始点 `origin` と通過点 `through` で半直線を表す集成体型。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域ともに $O(1)$。

## 注意点

`origin == through` となる退化した半直線を許容するかどうかは、利用する個々の API に従う。
