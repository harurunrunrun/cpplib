---
title: Exact Randomized Convex Polyhedron Intersection Test (厳密乱択凸多面体交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_polyhedron/convex_polyhedron_intersects_exact.hpp
---

## API

- `convex_polyhedron_intersects_exact(first, second)`: 両多面体の面を厳密な半空間に変換し、乱択増分法で共通部分の実行可能性を厳密に判定する。共通部分が空でなければ `true` を返す。乱数のシードは `std::random_device` から生成する。

## API別の時間計算量・空間計算量

$H = F_1 + F_2$ とする。制約の挿入順が入力と独立にランダムなので、期待時間・追加領域は $O(V_1+V_2+H)$、最悪時間は $O(V_1+V_2+H^3)$。多倍長整数演算のビット計算量は含まない。

## 注意点

- 両入力は `affine_dimension == 3` の閉じた有界凸多面体でなければならない。
- 非有限・空・退化・不正な面などの入力には `std::invalid_argument`、`std::out_of_range` または `std::domain_error` を送出する。`std::random_device` の生成失敗による例外も送出され得る。
- 乱数は実行時間にのみ影響し、戻り値には影響しない。シードを固定したい場合は `convex_polyhedron_intersects_exact_with_seed` を使う。
