---
title: 3D Segment and Tetrahedron Intersection (三次元線分と四面体の共通部分)
documentation_of: ../src/algorithm/geometry/3d/query/segment_tetrahedron_intersection.hpp
---

## API

- `segment_tetrahedron_intersection(segment, tetrahedron)`: 共通部分を空、点、線分として返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化線分を1点として扱う。退化四面体には `std::invalid_argument` を送出する。
