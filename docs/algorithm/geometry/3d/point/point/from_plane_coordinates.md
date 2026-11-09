---
title: From Plane Coordinates 3D (平面座標の三次元復元)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/point/from_plane_coordinates.hpp
---

## API

- `from_plane_coordinates(system, point)`: 平面座標系 `system` における二次元座標 `point` を三次元点に戻す。戻り値は `system.origin + system.first_axis * point.x + system.second_axis * point.y` である。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- `system` の 2 軸は正規直交していること（検査はしない）。
