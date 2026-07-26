---
title: Polygon3 to 2D Umbrella (三次元多角形の二次元化集約)
documentation_of: ../../../../src/algorithm/geometry/3d/polygon3_to_2d.hpp
---

## API

- `polygon3_to_2d(polygon, system)`: 指定した `PlaneCoordinateSystem3` で全頂点を二次元化する。
- `polygon3_to_2d(polygon)`: 座標系を自動構築し、座標系と二次元頂点列を返す。
- このヘッダは座標系指定あり・なしの2 leafをまとめてincludeする後方互換aggregatorである。

## API別の時間計算量・空間計算量

- 各overload: 頂点数を $N$ として時間 $O(N)$、結果を除く追加領域 $O(1)$、返却領域 $O(N)$。

## 注意点

必要な引数形式だけを使う場合は `polygon3_to_2d_with_plane_coordinate_system.hpp` または `polygon3_to_2d_without_plane_coordinate_system.hpp` を直接includeできる。
