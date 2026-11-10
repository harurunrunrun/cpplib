---
title: 3D Line and Triangle Intersection (三次元直線と三角形の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/line_triangle/line_triangle_intersection.hpp
---

## API

- `line_triangle_intersection(line, triangle)`: 直線と閉三角形の共通部分を `LinearIntersection3` で返す。結果は空集合（`std::monostate`）、点（`Point3`）、線分（`Segment3`）のいずれかである。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 三角形と同一平面上にある直線にも対応する。この場合は 3 辺との共通部分から結果を組み立てる。
- 退化した直線・三角形には `std::invalid_argument` を送出する。
