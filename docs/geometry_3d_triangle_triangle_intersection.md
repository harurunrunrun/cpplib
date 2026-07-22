---
title: 3D Triangle Intersection (三次元三角形同士の共通部分)
documentation_of: ../src/algorithm/geometry/3d/query/triangle_triangle_intersection.hpp
---

## API

- `triangle_triangle_intersection(first, second)`: 共通部分を空、点、線分、凸 `Polygon3` として返す。

## API別の時間計算量・空間計算量

- 三角形の頂点数は固定なので時間・追加領域 $O(1)$。

## 注意点

非共面・共面の両方に対応し、共面では凸polygon clippingを行う。退化三角形には `std::invalid_argument` を送出する。
