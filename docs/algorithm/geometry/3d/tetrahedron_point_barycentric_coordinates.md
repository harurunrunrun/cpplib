---
title: Tetrahedron-Point Barycentric Coordinates (四面体と点の重心座標)
documentation_of: ../../../../src/algorithm/geometry/3d/tetrahedron_point_barycentric_coordinates.hpp
---

## API

- `barycentric_coordinates(tetrahedron, point)`: 点を四面体の四頂点に対する重心座標で表す。

## API別の時間計算量・空間計算量

- 全API: 時間・追加領域 $O(1)$。

## 注意点

退化四面体または非有限入力では例外を送出する。
