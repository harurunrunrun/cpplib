---
title: 3D Containment (三次元包含判定)
documentation_of: ../src/algorithm/geometry/3d/contains.hpp
---

## API

- `contains(triangle, point)`: 点が閉三角形上にあるか返す。
- `contains(tetrahedron, point)`: 点が閉四面体内にあるか返す。
- `contains(sphere, point)`: 内部なら1、球面上なら0、外部なら-1を返す。

## API別の時間計算量・空間計算量

- 各overload: 時間・追加領域 $O(1)$。

## 注意点

退化三角形・四面体、または負半径の球には `std::invalid_argument` を送出する。
