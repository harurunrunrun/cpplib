---
title: 3D Line and Ray Intersection (三次元直線と半直線の共通部分)
documentation_of: ../src/algorithm/geometry/3d/query/line_ray_intersection.hpp
---

## API

- `line_ray_intersection(line, ray)`: 共通部分を `LinearIntersection3` で返す。空、点、半直線のいずれかである。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化直線または退化半直線には `std::invalid_argument` を送出する。半直線の始点を含む。
