---
title: Convex Polyhedron Membership Hierarchy Builder (凸多面体包含階層構築)
documentation_of: ../../../../src/algorithm/geometry/3d/convex_polyhedron_query_hierarchy_3d_build_membership_hierarchy.hpp
---

三次元包含query用の放射三角形階層を構築する内部member定義。

## API

- `ConvexPolyhedronQueryHierarchy3D::build_membership_hierarchy()`: 頂点重心から境界meshを放射投影し、縮約階層を構築するprivate member。

## API別の時間計算量・空間計算量

頂点数を $V$、面数を $F$ として時間 $O((V+F)\log(V+F))$、保持領域 $O(V+F)$。

## 注意点

private memberの実装leafであり、構築子からのみ利用する。入力検証と面向きの正規化は共有detail側で先に行われる。
