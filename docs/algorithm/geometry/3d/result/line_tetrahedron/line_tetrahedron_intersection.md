---
title: 3D Line and Tetrahedron Intersection (三次元直線と四面体の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/line_tetrahedron/line_tetrahedron_intersection.hpp
---

## API

- `line_tetrahedron_intersection(line, tetrahedron)`: 直線と閉四面体の共通部分を `LinearIntersection3` で返す。結果は空集合（`std::monostate`）、点（`Point3`）、線分（`Segment3`）のいずれかである。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 直線を `line.a + t * (line.b - line.a)` とパラメータ表示し、重心座標が非負となる 4 つの半空間でパラメータ $t$ の区間を切り詰めて求める。
- 退化した直線・四面体には `std::invalid_argument` を送出する。
