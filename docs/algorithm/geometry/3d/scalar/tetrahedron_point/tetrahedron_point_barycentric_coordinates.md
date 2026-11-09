---
title: Tetrahedron-Point Barycentric Coordinates (四面体と点の重心座標)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/tetrahedron_point/tetrahedron_point_barycentric_coordinates.hpp
---

## API

- `barycentric_coordinates(tetrahedron, point)`: 点 `point` を四面体の 4 頂点 $a, b, c, d$ に対する重心座標 $(w_a, w_b, w_c, w_d)$ （和が $1$）で表し、`std::array<long double, 4>` として返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- 点が四面体の外部にある場合は、負の成分を含む係数を返す。
- 退化した四面体（体積が $0$）や非有限な入力には `std::invalid_argument` を送出する。係数が `long double` で表現できない場合は `std::overflow_error` を送出する。
