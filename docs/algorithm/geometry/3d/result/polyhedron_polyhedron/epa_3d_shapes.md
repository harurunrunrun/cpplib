---
title: Expanding Polytope Algorithm 3D for Shapes (凸形状対の三次元拡張ポリトープ法)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron_polyhedron/epa_3d_shapes.hpp
---

2 つの凸多面体の貫通深度を、GJK と EPA（拡張ポリトープ法）で求める。

## API

- `epa_3d(first, second, tolerance = 1e-10L, max_iterations = 128)`: 2 個の凸多面体に GJK（反復上限 $96$）と EPA を順に適用し、`EPAResult3` を返す。

## API別の時間計算量・空間計算量

GJK の反復回数を $I_G$、EPA の反復回数を $I_E$、頂点数を $V_1, V_2$、EPA の最大面数を $F$ とすると、時間 $O(I_G(V_1+V_2) + I_E(V_1+V_2+F))$、追加空間 $O(V_1+V_2+F_1+F_2+I_E+F)$（$F_1, F_2$ は入力の面数。入力を面ごと複製・正規化したコピーを含む）。

## 注意点

- `tolerance` は形状の広がりに対する相対許容誤差である。
- 非正または非有限の `tolerance`、反復上限 $0$、空または非有限な入力、三次元の内部を持たない（`affine_dimension != 3` の）形状には `std::invalid_argument` を送出する。
- 結果を入力の座標系で表現できない場合は `std::overflow_error` を送出する。
