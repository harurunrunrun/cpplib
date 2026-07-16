---
title: 3D Circle and Plane Intersections (三次元円と平面の交点)
documentation_of: ../src/algorithm/geometry/3d/circle_plane_intersections.hpp
---

## API

- `circle_plane_intersections(circle, plane)`: 円周と平面の相異なる交点を0個から2個返す。

## API別の時間計算量・空間計算量

- 時間・出力以外の追加領域 $O(1)$。

## 注意点

Circle3 は中心・法線・半径が有限、法線が非零、半径が非負でなければならず、違反時は std::invalid_argument を送出する。

正半径の円全体が平面上にある場合は交点が無限個なので `std::domain_error` を送出する。半径0なら中心1点を返す。負半径または零法線には `std::invalid_argument` を送出する。
