---
title: Triangle-Point Barycentric Coordinates (三角形と点の重心座標)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/triangle_point/triangle_point_barycentric_coordinates.hpp
---

## API

- `barycentric_coordinates(triangle, point)`: 点 `point` を三角形の 3 頂点 $a, b, c$ に対する重心座標 $(w_a, w_b, w_c)$ （和が $1$）で表し、`std::array<long double, 3>` として返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- 点が三角形を含む平面の外にある場合は、その平面への正射影に対応する係数を返す。
- 退化した三角形や非有限な入力には `std::invalid_argument` を送出する。係数が `long double` で表現できない場合は `std::overflow_error` を送出する。
