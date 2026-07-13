---
title: 3D Coplanarity (三次元共面判定)
documentation_of: ../src/algorithm/geometry/3d/coplanar.hpp
---

## API

- `coplanar(first, second, third, fourth)`: 4点が同一平面上にあるか判定する。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

一致点や共線な点を含む退化配置も共面と判定する。
