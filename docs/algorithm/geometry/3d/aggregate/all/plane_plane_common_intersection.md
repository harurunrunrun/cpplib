---
title: 3D Plane-Plane Common Intersection (3次元2平面の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/plane_plane_common_intersection.hpp
---

2 平面の共通部分の結果型と、それを計算する関数をまとめて include する、互換のための集約ヘッダである。結果型と計算関数は、それぞれの個別ヘッダからも利用できる。

## API

- `PlanePlaneIntersection3`: `std::variant<std::monostate, Line3, Plane3>`。空集合・直線・平面のいずれかを表す。
- `plane_plane_common_intersection(first, second)`: 2 平面の共通部分を返す。平行で一致しなければ空集合、平行で一致すれば `first`、それ以外は交線を返す。

## API 別の時間計算量・空間計算量

型の操作と計算関数は、いずれも時間・追加空間ともに $O(1)$。

## 注意点

入力の平面は有限で、法線は非零でなければならない。非有限な入力には `std::invalid_argument` を送出する。
