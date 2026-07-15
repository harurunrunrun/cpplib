---
title: Point Side of 3D Plane (三次元平面に対する点の向き)
documentation_of: ../src/algorithm/geometry/3d/plane_side.hpp
---

## API

- `plane_side(plane, point)`: 法線側なら1、平面上なら0、反対側なら-1を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

零法線には `std::invalid_argument` を送出する。符号は `plane.normal` の向きに依存する。
