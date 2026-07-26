---
title: Triangle-Point Barycentric Coordinates (三角形と点の重心座標)
documentation_of: ../../../../src/algorithm/geometry/3d/triangle_point_barycentric_coordinates.hpp
---

## API

- `barycentric_coordinates(triangle, point)`: 点を三角形の三頂点に対する重心座標で表す。

## API別の時間計算量・空間計算量

- 全API: 時間・追加領域 $O(1)$。

## 注意点

退化三角形または非有限入力では例外を送出する。点が三角形の平面外でも射影に対応する係数を返す。
