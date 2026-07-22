---
title: Plane Coordinate System 3D (三次元平面座標系)
documentation_of: ../src/algorithm/geometry/3d/core/plane_coordinate_system.hpp
---

## API

- `PlaneCoordinateSystem3`: 原点、2本の平面内単位軸、単位法線を保持する。
- `plane_coordinate_system(plane)`: `plane`から右手系の正規直交座標系を構築する。

## API別の時間計算量・空間計算量

- すべて時間・追加領域 $O(1)$。

## 注意点

法線が零ベクトルの平面には`std::invalid_argument`を送出する。
