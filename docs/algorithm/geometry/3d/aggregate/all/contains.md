---
title: 3D Containment (三次元包含判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/contains.hpp
---

## API

- `contains(triangle, point)`: 点が閉三角形上にあるかを `bool` で返す。
- `contains(tetrahedron, point)`: 点が閉四面体内にあるかを `bool` で返す。
- `contains(sphere, point)`: `int` を返す。球の内部なら 1、球面上なら 0、外部なら $-1$ である。

## API 別の時間計算量・空間計算量

- 各オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 三角形版・四面体版は重心座標を用いて判定し、境界上の点も含む（許容誤差つきの符号判定）。三角形版は、まず点が三角形を含む平面上にあるかを判定する。
- 退化した三角形・四面体には `std::invalid_argument` を送出する。
- `Sphere3` は中心と半径が有限で、半径が非負でなければならない。違反時は `std::invalid_argument` を送出する。`point` が非有限の場合も `std::invalid_argument` を送出する。
