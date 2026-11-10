---
title: Exact SAT Convex Polyhedron Intersection Test (厳密分離軸凸多面体交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_polyhedron/convex_polyhedron_intersects_exact_sat.hpp
---

## API

- `convex_polyhedron_intersects_exact_sat(first, second)`: 両多面体の面法線と、両多面体の幾何的な辺の方向同士の外積をすべて分離軸の候補として調べ、共通部分が空でないかを決定的かつ厳密に判定する。

## API別の時間計算量・空間計算量

辺数を $E_1, E_2$、分離軸の候補数を $A = F_1 + F_2 + E_1 E_2$ として、時間 $O((F_1+F_2)\log(E_1+E_2) + A(V_1+V_2))$、追加領域 $O(V_1+V_2+E_1+E_2)$。多倍長整数演算のビット計算量は含まない。

## 注意点

- 両入力は `affine_dimension == 3` の閉じた有界凸多面体でなければならない。
- 空・非有限・退化した入力には `std::invalid_argument`、範囲外の頂点インデックスには `std::out_of_range`、有効な幾何的な辺が得られない入力には `std::domain_error` を送出する。退化面や非多様体辺を含む入力でも、辺の抽出で例外を送出する。
