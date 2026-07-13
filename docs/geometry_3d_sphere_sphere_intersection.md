---
title: 3D Sphere-Sphere Intersection (三次元球面同士の交円)
documentation_of: ../src/algorithm/geometry/geometry_3d/sphere_sphere_intersection.hpp
---

## API

- `sphere_sphere_intersection(first, second)`: 交円を返し、交わらなければ `nullopt`。接する場合は半径0の円を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

一致球では交円が一意でないため `std::domain_error`、負半径には `std::invalid_argument` を送出する。
