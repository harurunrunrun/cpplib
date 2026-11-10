---
title: Plane Coordinate System from Plane (平面からの3次元座標系構築)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/plane/plane_coordinate_system_from_plane.hpp
---

三次元平面から右手系の正規直交座標系を構築する。

## API

- `plane_coordinate_system(plane)`: `PlaneCoordinateSystem3{origin, first_axis, second_axis, normal}` を返す。`origin` は `plane.point`、`normal` は `plane.normal` を正規化した単位法線、`first_axis` と `second_axis` は平面上の単位軸で、`second_axis` は `normal` と `first_axis` の外積である。

## API別の時間計算量・空間計算量

時間・追加空間ともに $O(1)$。

## 注意点

`plane.normal` は有限な非零ベクトルでなければならない。法線が非有限または零ベクトルの場合は `std::invalid_argument` を送出する。
