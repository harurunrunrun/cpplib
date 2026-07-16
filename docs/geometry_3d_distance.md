---
title: 3D Distance (三次元距離)
documentation_of: ../src/algorithm/geometry/3d/distance.hpp
---

## API

- `distance(Point3, Point3)`: 点間距離。
- `distance(Line3, Point3)`, `distance(Ray3, Point3)`, `distance(Segment3, Point3)`, `distance(Plane3, Point3)`: 図形と点の距離。
- `distance(Line3, Line3)`, `distance(Segment3, Segment3)`: 同種図形間の距離。
- `distance(Triangle3, Point3)`: 閉三角形と点の距離。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

退化線分には対応する。直線・半直線・平面・三角形の退化条件に違反する入力と非有限入力には
`std::invalid_argument` を送出する。点差を局所scaleへ正規化し、最近点APIと同じ
候補から距離を安全に復元する。直線間距離は最近点の座標を構築せず、単位方向の外積から
直接求める。真の距離または必要な局所正規化差分を `long double` で表現できない場合は
`std::overflow_error` を送出する。
