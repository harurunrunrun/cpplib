---
title: Convex Polyhedron Intersection via Halfspaces (半空間による凸多面体共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/polyhedron_polyhedron/convex_polyhedron_intersection_via_halfspaces.hpp
---

両入力の外向き面を半空間に変換し、共通部分を構築する。

## API

- `convex_polyhedron_intersection_via_halfspaces(first, second)`: 厳密な交差判定の後、半空間の共通部分を `halfspace_intersection_3d` で構築して返す。共通部分が空なら空の多面体（`affine_dimension == -1`）を返す。低次元の接触も扱う。

## API 別の時間計算量・空間計算量

$V_i, F_i$ を各入力の頂点・面数、$H = F_1 + F_2$、出力サイズを $K$ とする。

- 空判定: 入力と独立な乱択順のもとで期待 $O(V_1 + V_2 + H)$、最悪 $O(V_1 + V_2 + H^3)$ 時間。
- full-dimensional な共通部分の構築: 期待 $O(H \log H + K)$ 時間、$O(H + K)$ 領域。
- 退化経路: 最悪 $O(H^4 + K)$ 時間、$O(H + K)$ 領域。

## 注意点

- どちらかの頂点列が空なら、空の多面体を返す。
- それ以外の入力は、`affine_dimension == 3` の有界凸多面体で、有限な頂点と外向きの非退化な面を持たなければならない。三次元でない入力や不正な面添字には `std::invalid_argument` を送出する。
- 共通部分が非有界と判定された場合は `std::domain_error` を送出する。
