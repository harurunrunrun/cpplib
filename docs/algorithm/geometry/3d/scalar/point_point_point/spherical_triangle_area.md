---
title: Spherical Triangle Area (球面三角形の面積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/point_point_point/spherical_triangle_area.hpp
---

## API

- `spherical_triangle_area(first, second, third, radius = 1)`: 3 頂点を短い方の大円弧で結んだ、半径 `radius` の球面上の三角形の面積（非負）を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 頂点は有限な非零ベクトル、半径は有限な非負値でなければならず、違反時は `std::invalid_argument` を送出する。頂点ベクトルの長さは無視する。
- 対蹠点の組を含むなどして境界が一意に定まらない場合は `std::domain_error` を送出する。
- 面積が `long double` で表現できない場合は `std::overflow_error` を送出する。
