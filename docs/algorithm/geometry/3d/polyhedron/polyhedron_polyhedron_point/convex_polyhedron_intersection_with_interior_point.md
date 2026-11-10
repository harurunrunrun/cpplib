---
title: Convex Polyhedron Intersection with Interior Point (狭義内点付き凸多面体共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/polyhedron_polyhedron_point/convex_polyhedron_intersection_with_interior_point.hpp
---

共通部分の狭義内点を利用して、半空間の共通部分として凸多面体の共通部分を構築する。

## API

- `convex_polyhedron_intersection_via_halfspaces_with_interior_point(first, second, strict_interior_point)`: 両入力の面の半空間と、呼び出し側が与えた共通部分の狭義内点から `ConvexPolyhedron3` を返す。内部では `halfspace_intersection_3d_with_interior_point` を呼ぶ。

## API 別の時間計算量・空間計算量

$H = F_1 + F_2$、出力サイズを $K$ として、期待時間 $O(H \log H + K)$、期待追加領域 $O(H + K)$。内部の固定シードに対する最悪時間は $O(H^2 \log H + K)$、最悪追加領域は $O(H^2 + K)$。

## 注意点

- 両入力は三次元（`affine_dimension == 3`）の有界凸多面体でなければならない。
- `strict_interior_point` は全入力半空間を狭義に満たす有限な点でなければならない。
- これらに違反する場合や不正な面添字には `std::invalid_argument` を送出する。
