---
title: Exact SAT Convex Polyhedron Intersection Test (厳密分離軸凸多面体交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_polyhedron/convex_polyhedron_intersects_exact_sat.hpp
---

## API

- `convex_polyhedron_intersects_exact_sat(first, second)`: 面法線と両多面体の幾何辺方向の外積を全て分離軸として調べ、共通部分が空でないかを決定的かつ厳密に返す。

## API別の時間計算量・空間計算量

軸数を $A=F_1+F_2+E_1E_2$ として時間 $O((F_1+F_2)\log(E_1+E_2)+A(V_1+V_2))$、追加領域 $O(V_1+V_2+E_1+E_2)$。多倍長整数のビット計算量は含まない。

## 注意点

両入力は完全三次元の閉じた有界凸多面体でなければならない。範囲外添字・退化面・非多様体辺・有効な幾何辺を得られない入力には例外を送出する。
