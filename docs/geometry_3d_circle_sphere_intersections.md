---
title: 3D Circle and Sphere Intersections (三次元円と球の交点)
documentation_of: ../src/algorithm/geometry/3d/point_collection/circle_sphere_intersections.hpp
---

## API

- `circle_sphere_intersections(circle, sphere)`: 円周と球面の相異なる交点を0個から2個返す。

## API別の時間計算量・空間計算量

- 時間・出力以外の追加領域 $O(1)$。

## 注意点

Sphere3 は中心・半径が有限かつ半径が非負でなければならず、違反時は std::invalid_argument を送出する。
Circle3 は中心・法線・半径が有限、法線が非零、半径が非負でなければならず、違反時は std::invalid_argument を送出する。

正半径の円全体が球面上にある場合は交点が無限個なので `std::domain_error` を送出する。負半径または円の零法線には `std::invalid_argument` を送出する。
