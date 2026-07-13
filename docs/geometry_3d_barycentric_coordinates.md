---
title: 3D Barycentric Coordinates (三次元重心座標)
documentation_of: ../src/algorithm/geometry/3d/barycentric_coordinates.hpp
---

## API

- `barycentric_coordinates(triangle, point)`: `{wa, wb, wc}` を返す。
- `barycentric_coordinates(tetrahedron, point)`: `{wa, wb, wc, wd}` を返す。

各重みの和は1で、頂点の重み付き和が `point` となる。

## API別の時間計算量・空間計算量

- 各overload: 時間・追加領域 $O(1)$。

## 注意点

三角形版は平面外の点も射影に相当する座標を返す。退化した三角形・四面体には `std::invalid_argument` を送出する。
