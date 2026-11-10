---
title: Polygon3 to 2D without Plane Coordinate System (平面座標系自動構築による三次元多角形の二次元化)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/polygon3_to_2d_without_plane_coordinate_system.hpp
---

## API

- `polygon3_to_2d(polygon)`: 多角形の先頭頂点と、面積ベクトルから求めた単位法線とで `PlaneCoordinateSystem3` を構築し、座標系と二次元頂点列の組（`std::pair<PlaneCoordinateSystem3, std::vector<Point>>`）を返す。

## API別の時間計算量・空間計算量

- `polygon3_to_2d`: 頂点数を $N$ として時間 $O(N)$、戻り値を除く追加領域 $O(1)$、戻り値の領域 $O(N)$。

## 注意点

頂点数が 3 未満の多角形、および面積ベクトルの長さが許容誤差の範囲で 0 になる多角形に対しては `std::invalid_argument` を送出する。任意の座標系を使う場合は `polygon3_to_2d_with_plane_coordinate_system.hpp` の `polygon3_to_2d(polygon, system)` を使う。
