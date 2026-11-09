---
title: Expanding Polytope Algorithm 3D with GJK Result (GJK結果付き三次元拡張ポリトープ法)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/epa_3d_with_gjk_result.hpp
---

## API

- `epa_3d(first, second, gjk_result, tolerance = 1.0e-10L, max_iterations = 128)`: 既存の `GJKResult3` の単体を初期単体として利用して EPA を実行し、`EPAResult3` を返す。

## API別の時間計算量・空間計算量

EPA の反復回数を $I_E$、2 つの凸多面体の頂点数を $V_1, V_2$、面数の最大値を $F$ とすると、時間 $O(I_E(V_1 + V_2 + F))$、追加領域 $O(I_E + F)$。
`gjk_result` の単体が空の場合は、反復回数の上限が定数（96 回）の GJK を追加で実行して補う。

## 注意点

- `gjk_result.intersects` が `false` なら、非交差を表すデフォルトの `EPAResult3` を返す。
- `gjk_result` の単体は入力形状と同じ座標系でなければならない。
- 許容誤差が正でないか有限でない場合、反復上限が $0$ の場合、入力が有限でない場合、三次元の内部を持たない形状の場合は `std::invalid_argument` を送出する。これらの検査は `gjk_result.intersects` の判定より前に行う。
