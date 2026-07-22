---
title: Closest Point on 3D Tetrahedron (三次元四面体上の最近点)
documentation_of: ../src/algorithm/geometry/3d/point/tetrahedron_closest_point.hpp
---

## API

- `closest_point(tetrahedron, point)`: 閉四面体上で点に最も近い点を返す。内部点は自身を返す。

## API別の時間計算量・空間計算量

- 4面を調べるため時間・追加領域 $O(1)$。

## 注意点

退化四面体には `std::invalid_argument` を送出する。
