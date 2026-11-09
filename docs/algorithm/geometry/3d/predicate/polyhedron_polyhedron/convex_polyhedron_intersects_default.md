---
title: Convex Polyhedron Intersection Test by GJK (GJK法による凸多面体交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_polyhedron/convex_polyhedron_intersects_default.hpp
---

## API

- `convex_polyhedron_intersects(first, second, tolerance = 1e-12L, max_iterations = 96)`: Minkowski 差に対する GJK 法で、2 つの凸多面体の共通部分（境界での接触を含む）が空でないかを許容誤差付きで判定する。`gjk_intersects_3d` をそのまま呼び出す。

## API別の時間計算量・空間計算量

頂点数を $V_1, V_2$、面数を $F_1, F_2$、実際の反復回数を $I$ として時間 $O(V_1+V_2+F_1+F_2+I(V_1+V_2))$、追加領域 $O(V_1+V_2+F_1+F_2)$（正規化のために入力を複製する）。

## 注意点

- 入力の頂点列は空でなく有限で、それぞれの凸包が対象の凸形状を表す必要がある。頂点列が空または非有限なら `std::invalid_argument` を送出する。
- `tolerance` は正の有限値、`max_iterations` は正でなければならず、違反時は `std::invalid_argument` を送出する。
- `tolerance` は、共通の平行移動を除いた両形状の広がりで正規化した座標に対する相対許容誤差である。
- 反復上限に達した場合もその時点の単体から結果を返す、実用的な近似判定であり、厳密判定ではない。厳密判定が必要なら `convex_polyhedron_intersects_exact` などを使う。
