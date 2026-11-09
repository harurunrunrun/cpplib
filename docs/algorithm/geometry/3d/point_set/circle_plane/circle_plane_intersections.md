---
title: 3D Circle and Plane Intersections (三次元円と平面の交点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/circle_plane/circle_plane_intersections.hpp
---

## API

- `circle_plane_intersections(circle, plane)`: 円周と平面の相異なる交点を、$0$ 個から $2$ 個の `std::vector<Point3>` で返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力以外の追加領域 $O(1)$。

## 注意点

- `Circle3` は中心・法線・半径が有限、法線が非零、半径が非負でなければならない。違反時は `std::invalid_argument` を送出する。 平面が非有限または零法線の場合も `std::invalid_argument` を送出する。
- 円の平面と `plane` が平行で一致しない場合は空を返す。
- 正半径の円全体が平面上にある場合は交点が無限個なので `std::domain_error` を送出する。半径 $0$ の円が平面上にあれば中心 1 点を返す。
