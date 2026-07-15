---
title: From Plane Coordinates 3D (平面座標の三次元復元)
documentation_of: ../src/algorithm/geometry/3d/from_plane_coordinates.hpp
---

## API

- `from_plane_coordinates(system, point)`: 2次元平面座標を三次元点へ戻す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

`system`の軸は正規直交していること。
