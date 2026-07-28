---
title: Seeded Exact Convex Polyhedron Intersection Test (seed指定厳密凸多面体交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_polyhedron/convex_polyhedron_intersects_exact_with_seed.hpp
---

## API

- `convex_polyhedron_intersects_exact_with_seed(first, second, random_seed)`: seedで決めた制約順を使い、二つの凸多面体の共通部分が空でないかを厳密判定する。

## API別の時間計算量・空間計算量

$H=F_1+F_2$ としてseedが入力と独立なら期待時間・追加領域 $O(V_1+V_2+H)$、固定seedに対する最悪時間 $O(V_1+V_2+H^3)$。

## 注意点

seedは実行時間だけに影響し、返り値には影響しない。両入力は完全三次元の閉じた有界凸多面体でなければならず、不正入力には `std::invalid_argument`、`std::out_of_range` または `std::domain_error` を送出する。
