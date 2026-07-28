---
title: Plane Coordinate System from Plane (平面からの3次元座標系構築)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/plane/plane_coordinate_system_from_plane.hpp
---

3次元平面から右手系の正規直交座標系を構築します。

## API

- `plane_coordinate_system(plane)`: 平面上の原点、2本の単位軸、単位法線を持つ `PlaneCoordinateSystem3` を返します。

## API別の時間計算量・空間計算量

時間・追加空間ともに $O(1)$ です。

## 注意点

`plane.normal` は有限な非零ベクトルでなければなりません。不正な平面には `std::invalid_argument` を送出します。