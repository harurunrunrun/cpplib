---
title: Point Side of 3D Plane (三次元平面に対する点の向き)
documentation_of: ../../../../../../src/algorithm/geometry/3d/integer/plane_point/plane_side.hpp
---

## API

- `plane_side(plane, point)`: 点が法線の側にあれば `1`、平面上にあれば `0`、反対側にあれば `-1` を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

符号付き距離を絶対許容誤差 `GEOMETRY3D_EPS` で判定するため、距離の絶対値が `GEOMETRY3D_EPS` 以下なら平面上とみなす。符号は `plane.normal` の向きに依存する。
法線が零ベクトルの場合や非有限な入力には `std::invalid_argument` を、符号付き距離を `long double` で表現できない場合は `std::overflow_error` を送出する。
