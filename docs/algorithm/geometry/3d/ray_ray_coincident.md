---
title: 3D Ray-Ray Coincidence (三次元半直線同士の一致判定)
documentation_of: ../../../../src/algorithm/geometry/3d/ray_ray_coincident.hpp
---

## API

- `coincident(first, second)`: 2本の `Ray3` が同じ始点と同じ向きを持ち、点集合として一致するなら `true` を返す。

## API別の時間計算量・空間計算量

- `coincident`: 時間・追加領域 $O(1)$。

## 注意点

支持直線が同じでも向きが反対なら `false`。いずれかが退化半直線なら `std::invalid_argument` を送出する。
