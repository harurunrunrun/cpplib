---
title: 3D Line and Tetrahedron Intersection (三次元直線と四面体の共通部分)
documentation_of: ../src/algorithm/geometry/3d/query/line_tetrahedron_intersection.hpp
---

## API

- `line_tetrahedron_intersection(line, tetrahedron)`: 閉四面体との共通部分を空、点、線分として返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

重心座標の4半空間で直線parameterをclipする。退化直線・退化四面体には `std::invalid_argument` を送出する。
