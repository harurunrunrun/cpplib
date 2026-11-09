---
title: Seeded Exact Convex Polyhedron Intersection Test (seed指定厳密凸多面体交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_polyhedron/convex_polyhedron_intersects_exact_with_seed.hpp
---

## API

- `convex_polyhedron_intersects_exact_with_seed(first, second, random_seed)`: `random_seed`（`std::uint64_t`）で決まる制約順を用いて、2 つの凸多面体の共通部分が空でないかを厳密に判定する。

## API別の時間計算量・空間計算量

$H = F_1 + F_2$ とする。シードが入力と独立なら期待時間・追加領域は $O(V_1+V_2+H)$、固定シードに対する最悪時間は $O(V_1+V_2+H^3)$。

## 注意点

- シードは実行時間にのみ影響し、戻り値には影響しない。同じ入力とシードでは実行内容も同じである。
- 両入力は `affine_dimension == 3` の閉じた有界凸多面体でなければならず、不正な入力には `std::invalid_argument`、`std::out_of_range` または `std::domain_error` を送出する。
