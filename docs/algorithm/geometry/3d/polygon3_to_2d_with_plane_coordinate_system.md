---
title: Polygon3 to 2D with Plane Coordinate System (平面座標系指定による三次元多角形の二次元化)
documentation_of: ../../../../src/algorithm/geometry/3d/polygon3_to_2d_with_plane_coordinate_system.hpp
---

## API

- `polygon3_to_2d(polygon, system)`: 指定した `PlaneCoordinateSystem3` の2基底との内積で、全頂点を二次元の `Point` 列へ変換する。

## API別の時間計算量・空間計算量

- `polygon3_to_2d`: 頂点数を $N$ として時間 $O(N)$、結果を除く追加領域 $O(1)$、返却領域 $O(N)$。

## 注意点

このoverloadは座標系の構築、多角形の頂点数・面積・共面性の検査を行わない。有効な `PlaneCoordinateSystem3` を呼出側で用意する。
