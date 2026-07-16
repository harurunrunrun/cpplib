---
title: Point on 3D Sphere (三次元球面上判定)
documentation_of: ../src/algorithm/geometry/3d/on_sphere.hpp
---

## API

- `on_sphere(sphere, point)`: 点が球面上にあるかを返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

Sphere3 は中心・半径が有限かつ半径が非負でなければならず、違反時は std::invalid_argument を送出する。

負半径には `std::invalid_argument` を送出する。半径0の球面は中心1点である。
