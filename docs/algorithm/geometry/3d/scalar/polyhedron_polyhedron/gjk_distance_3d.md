---
title: GJK Distance 3D (三次元 GJK 距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/polyhedron_polyhedron/gjk_distance_3d.hpp
---

## API

- `gjk_distance_3d(first, second, tolerance = 1e-12L, max_iterations = 96)`: GJK 法で 2 つの凸形状間のユークリッド距離を返す。交差する場合は $0$ を返す。`gjk_query_3d(...).distance` と同じ値である。

## API別の時間計算量・空間計算量

- 反復回数を $I$、各形状の頂点数を $V_1, V_2$ として、時間 $O(I(V_1 + V_2))$、追加領域 $O(1)$。

## 注意点

- 前提と例外は `gjk_query_3d` と同じである。
- 許容誤差 `tolerance` は両形状の広がりに対する相対値である。
- 結果は入力座標系での長さで、表現できない場合は `std::overflow_error` を送出する。
