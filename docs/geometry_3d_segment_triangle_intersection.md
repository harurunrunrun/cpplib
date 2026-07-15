---
title: 3D Segment and Triangle Intersection (三次元線分と三角形の共通部分)
documentation_of: ../src/algorithm/geometry/3d/segment_triangle_intersection.hpp
---

## API

- `segment_triangle_intersection(segment, triangle)`: 2つの閉集合の共通部分を空、点、線分として返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化線分を1点として扱う。退化三角形には `std::invalid_argument` を送出する。
