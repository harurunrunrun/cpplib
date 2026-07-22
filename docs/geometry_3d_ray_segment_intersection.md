---
title: 3D Ray and Segment Intersection (三次元半直線と線分の共通部分)
documentation_of: ../src/algorithm/geometry/3d/query/ray_segment_intersection.hpp
---

## API

- `ray_segment_intersection(ray, segment)`: 共通部分を `LinearIntersection3` で返す。空、点、線分のいずれかである。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化線分を1点として扱う。退化半直線には `std::invalid_argument` を送出する。
