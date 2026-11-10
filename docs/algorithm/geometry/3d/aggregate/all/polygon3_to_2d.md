---
title: Polygon3 to 2D Umbrella (三次元多角形の二次元化集約)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/polygon3_to_2d.hpp
---

このヘッダは、座標系を指定する版と指定しない版の 2 つの個別ヘッダをまとめて include する、後方互換のための集約ヘッダである。必要な引数の形式だけを使う場合は、`polygon3_to_2d_with_plane_coordinate_system.hpp` または `polygon3_to_2d_without_plane_coordinate_system.hpp` を直接 include すればよい。

## API

- `polygon3_to_2d(polygon, system)`: 指定した `PlaneCoordinateSystem3` で全頂点を二次元座標に変換し、`std::vector<Point>` を返す。
- `polygon3_to_2d(polygon)`: 座標系を自動で構築し、座標系と二次元の頂点列の組 `std::pair<PlaneCoordinateSystem3, std::vector<Point>>` を返す。

## API 別の時間計算量・空間計算量

- 各オーバーロード: 頂点数を $N$ として時間 $O(N)$、戻り値を除く追加空間 $O(1)$、戻り値の空間 $O(N)$。

## 注意点

座標系を自動で構築する版では、頂点数が 3 未満なら `std::invalid_argument` を送出する。
