---
title: 3D Line and Ray Intersection (三次元直線と半直線の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/line_ray/line_ray_intersection.hpp
---

## API

- `line_ray_intersection(line, ray)`: 直線と半直線の共通部分を `LinearIntersection3` で返す。結果は空集合（`std::monostate`）、点（`Point3`）、半直線（`Ray3`、`ray` そのもの）のいずれかである。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 半直線は始点を含む。
- 退化した直線・半直線には `std::invalid_argument` を送出する。
