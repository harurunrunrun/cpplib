---
title: Plane Coordinate System 3D (3次元平面座標系)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/plane_coordinate_system.hpp
---

平面座標系の型と、平面から座標系を構築する関数をまとめて include する、互換のための集約ヘッダである。型と構築関数は、それぞれの個別ヘッダからも利用できる。

## API

- `PlaneCoordinateSystem3`: 原点 `origin`、平面内の 2 本の単位軸 `first_axis`, `second_axis`、単位法線 `normal` を保持する。
- `plane_coordinate_system(plane)`: `plane` から右手系の正規直交座標系を返す。原点は `plane.point` である。

## API 別の時間計算量・空間計算量

型の操作と構築関数は、いずれも時間・追加空間ともに $O(1)$。

## 注意点

平面の法線は有限な非零ベクトルでなければならない。
