---
title: 3D Distance (三次元距離)
documentation_of: ../src/algorithm/geometry/geometry_3d/distance.hpp
---

## API

- `distance(Point3, Point3)`: 点間距離。
- `distance(Line3, Point3)`, `distance(Ray3, Point3)`, `distance(Segment3, Point3)`, `distance(Plane3, Point3)`: 図形と点の距離。
- `distance(Line3, Line3)`, `distance(Segment3, Segment3)`: 同種図形間の距離。
- `distance(Triangle3, Point3)`: 閉三角形と点の距離。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

退化線分には対応する。直線・半直線・平面・三角形の退化条件に違反すると `std::invalid_argument` を送出する。
