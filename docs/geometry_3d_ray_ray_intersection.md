---
title: 3D Ray Intersection (三次元半直線同士の共通部分)
documentation_of: ../src/algorithm/geometry/3d/query/ray_ray_intersection.hpp
---

## API

- `ray_ray_intersection(first, second)`: 共通部分を `LinearIntersection3` で返す。空、点、線分、半直線のいずれかである。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

同一直線上で同方向なら半直線、向かい合って重なるなら線分になる。退化半直線には `std::invalid_argument` を送出する。
