---
title: Convex Polyhedron Intersection via Halfspaces (半空間による凸多面体共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/polyhedron_polyhedron/convex_polyhedron_intersection_via_halfspaces.hpp
---

両入力の外向き面を半空間へ変換し、共通部分を構築する。

## API

- `convex_polyhedron_intersection_via_halfspaces(first, second)`: exactな実行可能性判定後に半空間共通部分を返す。空なら空の多面体を返し、低次元接触も扱う。

## API別の時間計算量・空間計算量

$H=F_1+F_2$、出力sizeを $K$ とする。空判定は入力独立な乱択順で期待 $O(V_1+V_2+H)$、最悪 $O(V_1+V_2+H^3)$。full-dimensionalな共通部分の構築は期待 $O(H\log H+K)$ 時間、$O(H+K)$ 領域。退化経路は最悪 $O(H^4+K)$ 時間、$O(H+K)$ 領域。

## 注意点

両入力は `affine_dimension == 3` の非空な有界凸多面体で、有限頂点と外向き非退化面を持つ必要がある。不正な面添字には `std::invalid_argument`、非有界判定には `std::domain_error` を送出する。
