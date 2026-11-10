---
title: To Plane Coordinates 3D (三次元点の平面座標変換)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/point/to_plane_coordinates.hpp
---

## API

- `to_plane_coordinates(system, point)`: 三次元点 `point` を平面座標系 `system` の二次元座標へ写す。各座標は `point - system.origin` と各軸の内積であり、平面外の成分は捨てる。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- `system` の 2 軸は正規直交していること（検査はしない）。
