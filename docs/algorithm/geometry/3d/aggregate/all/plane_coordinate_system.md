---
title: Plane Coordinate System 3D (3次元平面座標系)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/plane_coordinate_system.hpp
---

平面座標系の型と平面からの構築関数をまとめて読み込む互換aggregatorです。

## API

- `PlaneCoordinateSystem3`: 原点、2本の平面内単位軸、単位法線を保持します。
- `plane_coordinate_system(plane)`: `plane` から右手系の正規直交座標系を返します。

## API別の時間計算量・空間計算量

型操作と構築関数はいずれも時間・追加空間 $O(1)$ です。

## 注意点

従来の一括include用headerです。型と構築関数は個別leafからも利用できます。平面の法線は有限な非零ベクトルでなければなりません。