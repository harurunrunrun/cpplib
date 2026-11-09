---
title: GJK Intersection 3D (3次元GJK交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_polyhedron/gjk_intersects_3d.hpp
---

## API

- `gjk_intersects_3d(first, second, tolerance = 1e-12L, max_iterations = 96)`: 2 つの凸形状が接触または交差しているかを返す。`gjk_query_3d(first, second, tolerance, max_iterations).intersects` と同じである。

## API別の時間計算量・空間計算量

- 頂点数を $V_1, V_2$、面数を $F_1, F_2$、実際の反復回数を $I$ として時間 $O(V_1+V_2+F_1+F_2+I(V_1+V_2))$、追加領域 $O(V_1+V_2+F_1+F_2)$（`gjk_query_3d` が正規化のために入力を複製する）。

## 注意点

- 前提条件と例外は `gjk_query_3d` と同じである。頂点列は空でなく有限で、`tolerance` は正の有限値、`max_iterations` は正でなければならず、違反時は `std::invalid_argument` を送出する。
- 許容誤差以内の接触も交差とみなす。
- 許容誤差は、共通の平行移動を除いた両形状の広がりに対する相対値である。
