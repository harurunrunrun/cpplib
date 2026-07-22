---
title: 2D Squared Norm (二次元ノルム二乗)
documentation_of: ../src/algorithm/geometry/2d/scalar/norm.hpp
---

## API

- `norm(point)`: 原点からのユークリッド距離の二乗を返す。

## API別の時間計算量・空間計算量

- `norm`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

平方根を取らない。長さには `abs(point)` を用いる。
