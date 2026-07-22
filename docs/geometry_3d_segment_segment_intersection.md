---
title: 3D Segment Intersection (三次元線分同士の共通部分)
documentation_of: ../src/algorithm/geometry/3d/query/segment_segment_intersection.hpp
---

## API

- `segment_segment_intersection(first, second)`: 共通部分を `LinearIntersection3` で返す。空、点、線分のいずれかである。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

両方の退化線分を含めて1点として処理する。端点接触は点、正の長さを持つ重なりは線分で返す。
