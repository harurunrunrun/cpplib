---
title: 3D Line and Triangle Intersection (三次元直線と三角形の共通部分)
documentation_of: ../src/algorithm/geometry/3d/line_triangle_intersection.hpp
---

## API

- `line_triangle_intersection(line, triangle)`: 閉三角形との共通部分を空、点、線分として返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

三角形と共面な直線にも対応する。退化直線・退化三角形には `std::invalid_argument` を送出する。
