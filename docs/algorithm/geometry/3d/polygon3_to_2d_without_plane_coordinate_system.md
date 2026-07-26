---
title: Polygon3 to 2D without Plane Coordinate System (平面座標系自動構築による三次元多角形の二次元化)
documentation_of: ../../../../src/algorithm/geometry/3d/polygon3_to_2d_without_plane_coordinate_system.hpp
---

## API

- `polygon3_to_2d(polygon)`: 多角形の先頭頂点と面積ベクトルから `PlaneCoordinateSystem3` を構築し、座標系と二次元頂点列の組を返す。

## API別の時間計算量・空間計算量

- `polygon3_to_2d`: 頂点数を $N$ として時間 $O(N)$、結果を除く追加領域 $O(1)$、返却領域 $O(N)$。

## 注意点

3頂点未満または面積が零の多角形には `std::invalid_argument` を送出する。任意の座標系を使う場合は `polygon3_to_2d_with_plane_coordinate_system.hpp` を使う。
