---
title: Expanding Polytope Algorithm 3D for Shapes (凸形状対の三次元拡張ポリトープ法)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron_polyhedron/epa_3d_shapes.hpp
---

## API

- `epa_3d(first, second, tolerance=1e-10L, max_iterations=128)`: 2個の凸多面体にGJKとEPAを順に適用し、`EPAResult3` を返す。

## API別の時間計算量・空間計算量

GJK反復数を $I_G$、EPA反復数を $I_E$、頂点数を $V_1,V_2$、EPAの最大面数を $F$ とすると、時間 $O(I_G(V_1+V_2)+I_E(V_1+V_2+F))$、追加領域 $O(I_E+F)$。

## 注意点

`tolerance` は形状の広がりに対する相対許容誤差である。非正または非有限の許容誤差、反復上限0、非有限入力、三次元内部を持たない形状には `std::invalid_argument` を送出する。結果を入力座標系で表現できなければ `std::overflow_error` を送出する。
