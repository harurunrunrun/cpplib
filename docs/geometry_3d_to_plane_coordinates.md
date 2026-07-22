---
title: To Plane Coordinates 3D (三次元点の平面座標変換)
documentation_of: ../src/algorithm/geometry/3d/point/to_plane_coordinates.hpp
---

## API

- `to_plane_coordinates(system, point)`: 三次元点を`system`の2次元座標へ写す。平面外成分は捨てる。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

`system`の軸は正規直交していること。
