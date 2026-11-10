---
title: 3D Circle and Sphere Intersections (三次元円と球の交点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/circle_sphere/circle_sphere_intersections.hpp
---

## API

- `circle_sphere_intersections(circle, sphere)`: 円周と球面の相異なる交点を、$0$ 個から $2$ 個の `std::vector<Point3>` で返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力以外の追加領域 $O(1)$。

## 注意点

- `Circle3` は中心・法線・半径が有限、法線が非零、半径が非負でなければならない。違反時は `std::invalid_argument` を送出する。
- `Sphere3` は中心・半径が有限で、半径が非負でなければならない。違反時は `std::invalid_argument` を送出する。
- 円の平面と球の交円を求め、`circle_circle_intersections` で円同士の交点に帰着する。
- 正半径の円全体が球面上にある場合は交点が無限個なので `std::domain_error` を送出する。
