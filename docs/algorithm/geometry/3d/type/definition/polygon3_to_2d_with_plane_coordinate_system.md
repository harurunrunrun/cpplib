---
title: Polygon3 to 2D with Plane Coordinate System (平面座標系指定による三次元多角形の二次元化)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/polygon3_to_2d_with_plane_coordinate_system.hpp
---

## API

- `polygon3_to_2d(polygon, system)`: 指定した `PlaneCoordinateSystem3` の 2 本の基底との内積により、全頂点を二次元の `Point` の列（`std::vector<Point>`）へ変換する。

## API別の時間計算量・空間計算量

- `polygon3_to_2d`: 頂点数を $N$ として時間 $O(N)$、戻り値を除く追加領域 $O(1)$、戻り値の領域 $O(N)$。

## 注意点

このオーバーロードは座標系を構築せず、多角形の頂点数・面積・共面性も検査しない。有効な `PlaneCoordinateSystem3` は呼び出し側で用意する。
