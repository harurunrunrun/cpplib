---
title: Convex Polyhedron Membership Hierarchy Builder (凸多面体包含階層構築)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron/convex_polyhedron_query_hierarchy_3d_build_membership_hierarchy.hpp
---

`ConvexPolyhedronQueryHierarchy3D` の内部メンバ関数の定義で、三次元の包含クエリに用いる放射三角形階層を構築する。

## API

- `ConvexPolyhedronQueryHierarchy3D::build_membership_hierarchy()`: 頂点の座標和と頂点数を厳密な 2 進有理数（dyadic）で保持し、頂点重心から見た方向へ境界メッシュを放射投影して、縮約階層（`RadialHierarchy`）を構築する private メンバ関数。

## API別の時間計算量・空間計算量

頂点数を $V$、面数を $F$ とすると、時間 $O((V+F) \log (V+F))$、保持する領域は $O(V+F)$。

## 注意点

private メンバ関数の実装を分割したヘッダであり、コンストラクタからのみ利用する。入力の検証と面の向きの正規化は、共有の detail 側で先に行われる。
