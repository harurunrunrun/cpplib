---
title: 3D Tetrahedron Intersection (三次元四面体同士の共通部分)
documentation_of: ../src/algorithm/geometry/3d/tetrahedron_tetrahedron_intersection.hpp
---

## API

- `tetrahedron_tetrahedron_intersection(first, second)`: 共通部分の頂点を列挙して `ConvexPolyhedron3` として返す。

## API別の時間計算量・空間計算量

- 候補数は定数で、凸包を含め時間・追加領域 $O(1)$。

## 注意点

`affine_dimension` は空なら-1、点0、線分1、平面polygon2、立体3である。退化四面体には `std::invalid_argument` を送出する。
