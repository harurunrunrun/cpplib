---
title: 3D Squared Norm (三次元ノルム二乗)
documentation_of: ../src/algorithm/geometry/3d/scalar/norm.hpp
---

## API

- `norm(point)`: ベクトル長の二乗を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

平方根を取らない。長さには `abs(point)` を用いる。
