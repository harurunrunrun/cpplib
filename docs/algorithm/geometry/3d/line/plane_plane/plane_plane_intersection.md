---
title: 3D Plane-Plane Intersection (三次元平面同士の交線)
documentation_of: ../../../../../../src/algorithm/geometry/3d/line/plane_plane/plane_plane_intersection.hpp
---

## API

- `plane_plane_intersection(first, second)`: 2 平面の一意な交線を、交線上の相異なる 2 点を持つ `Line3` で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

平行または一致する平面には `std::domain_error`、法線が零ベクトルの平面・非有限な入力には `std::invalid_argument` を送出する。
平行判定と二元連立一次方程式は、入力の法線そのものの正確な 2 進有理数（dyadic）値で解く。返す 2 点は、いずれも両平面上にあると判定される候補から選ぶ。交線を表す有限な 2 点を `long double` で表現できない場合は `std::overflow_error` を送出する。
