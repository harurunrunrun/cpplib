---
title: Convex Polyhedron Intersection Test by GJK (GJK法による凸多面体交差判定)
documentation_of: ../../../../src/algorithm/geometry/3d/convex_polyhedron_intersects_default.hpp
---

## API

- `convex_polyhedron_intersects(first, second, tolerance, max_iterations)`: Minkowski差に対するGJK法で、境界接触を含む共通部分が空でないかを許容誤差付きで返す。

## API別の時間計算量・空間計算量

頂点数を $V_i$、面数を $F_i$、実行反復数を $I$ として時間 $O(F_1+F_2+I(V_1+V_2))$、追加領域 $O(V_1+V_2+F_1+F_2)$。

## 注意点

入力頂点列は空でなく有限で、それぞれの凸包を表す必要がある。`tolerance` は正の有限値、`max_iterations` は正でなければならない。反復上限時にも現在のsimplexから返す実用的な近似判定であり、厳密判定ではない。
