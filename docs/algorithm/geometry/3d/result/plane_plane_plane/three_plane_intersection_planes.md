---
title: Common Intersection of Three Planes (3平面の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/plane_plane_plane/three_plane_intersection_planes.hpp
---

3 個の三次元平面すべてに共通する集合を計算する。

## API

- `three_plane_intersection(first, second, third)`: 共通部分を `ThreePlaneIntersection3` として返す。空集合なら `std::monostate`、一点なら `Point3`、直線なら `Line3`、3 平面が一致すれば `first` を `Plane3` として保持する。

## API別の時間計算量・空間計算量

時間・追加空間ともに $O(1)$。

## 注意点

各平面の点と法線は有限値で、法線は非零でなければならない。違反した入力には、内部で呼ぶ平面演算（法線の正規化・点と平面の判定・2 平面の交線計算）が `std::invalid_argument` を送出する。
