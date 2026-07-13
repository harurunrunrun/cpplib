---
title: 3D Vector Length (三次元ベクトル長)
documentation_of: ../src/algorithm/geometry/3d/abs.hpp
---

## API

- `abs(point)`: ベクトルのユークリッド長を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

`std::sqrt(norm(point))` を計算する。
