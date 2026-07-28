---
title: Convex Polyhedron Intersection with Interior Point (狭義内点付き凸多面体共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/polyhedron_polyhedron_point/convex_polyhedron_intersection_with_interior_point.hpp
---

共通部分の狭義内点を利用して半空間共通部分を構築する。

## API

- `convex_polyhedron_intersection_via_halfspaces_with_interior_point(first, second, strict_interior_point)`: 両入力面の半空間と、呼出側が与えた共通部分の狭義内点から `ConvexPolyhedron3` を返す。

## API別の時間計算量・空間計算量

$H=F_1+F_2$、出力sizeを $K$ として期待時間 $O(H\log H+K)$、期待追加領域 $O(H+K)$。固定seedに対する最悪時間 $O(H^2\log H+K)$、最悪追加領域 $O(H^2+K)$。

## 注意点

両入力は完全三次元の有界凸多面体でなければならない。`strict_interior_point` は全入力半空間を狭義に満たす有限点でなければならず、違反時は `std::invalid_argument` を送出する。
