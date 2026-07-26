---
title: Convex Polyhedron Query Hierarchy Internal Detail (凸多面体クエリ階層内部実装)
documentation_of: ../../../../src/algorithm/geometry/3d/convex_polyhedron_query_hierarchy_3d_detail.hpp
---

exact predicate、放射階層、AABB階層、および `ConvexPolyhedronQueryHierarchy3D` の共有本体を保持する。

## API

- `ConvexPolyhedronQueryHierarchy3D(polyhedron, leaf_capacity)`: 検証済み多面体を所有し、支持点・包含・最近点query用の階層を構築する。
- `support_point`, `contains`, `closest_point`: 共有本体に定義された公開query。
- `convex_polyhedron_query_hierarchy_3d_detail` 内の型・関数: exact arithmeticと階層構築用の内部要素。

## API別の時間計算量・空間計算量

構築は $N=V+F$ として $O(N\log N)$ 時間・$O(N)$ 領域。三次元の支持点は $O(\log F)$、包含は $O(\log V)$、最近点は訪問AABB面node数を $K_F$ として $O(\log V+K_F)$、最悪 $O(F)$。

## 注意点

直接include用の安定APIではなく、4つのmember実装leafから共有される。利用側は元aggregatorをincludeする。座標・queryは有限で、三次元入力は閉じた一貫向きの凸meshでなければならない。
