---
title: Expanding Polytope Algorithm 3D with GJK Result (GJK結果付き三次元拡張ポリトープ法)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/epa_3d_with_gjk_result.hpp
---

## API

- `epa_3d(first, second, gjk_result, tolerance=1e-10L, max_iterations=128)`: 既存の `GJKResult3` を初期単体として利用し、`EPAResult3` を返す。

## API別の時間計算量・空間計算量

EPA反復数を $I_E$、頂点数を $V_1,V_2$、最大面数を $F$ とすると、時間 $O(I_E(V_1+V_2+F))$、追加領域 $O(I_E+F)$。空の単体を補完する場合は定数上限付きGJKを追加実行する。

## 注意点

`gjk_result.intersects` が偽なら非交差結果を返す。単体は入力形状と同じ座標系でなければならない。非正または非有限の許容誤差、反復上限0、非有限入力、三次元内部を持たない形状には `std::invalid_argument` を送出する。
