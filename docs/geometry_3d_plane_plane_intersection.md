---
title: 3D Plane-Plane Intersection (三次元平面同士の交線)
documentation_of: ../src/algorithm/geometry/3d/shape/plane_plane_intersection.hpp
---

## API

- `plane_plane_intersection(first, second)`: 2平面の一意な交線を `Line3` で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

平行または一致する平面には `std::domain_error`、零法線・非有限入力には
`std::invalid_argument` を送出する。平行判定と2元一次方程式はraw法線のexact
dyadic値で行う。返す2点はいずれも両平面上となる候補を選ぶ。交線を構成する有限な
2点を `long double` で表現できない場合は `std::overflow_error` を送出する。
