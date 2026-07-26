---
title: Exact Randomized Convex Polyhedron Intersection Test (厳密乱択凸多面体交差判定)
documentation_of: ../../../../src/algorithm/geometry/3d/convex_polyhedron_intersects_exact.hpp
---

## API

- `convex_polyhedron_intersects_exact(first, second)`: 面をexactな半空間へ変換し、ランダム化増分法で共通部分の実行可能性を厳密判定する。乱数源は `std::random_device` から生成する。

## API別の時間計算量・空間計算量

$H=F_1+F_2$ として入力独立な乱択順で期待時間・追加領域 $O(V_1+V_2+H)$、最悪時間 $O(V_1+V_2+H^3)$。

## 注意点

両入力は `affine_dimension == 3` の閉じた有界凸多面体でなければならない。非有限・空・退化・不正な面には `std::invalid_argument`、`std::out_of_range` または `std::domain_error` を送出する。`std::random_device` の生成失敗も送出され得る。
