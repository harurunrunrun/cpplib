---
title: Convex Polyhedron Support Hierarchy Builder (凸多面体支持点階層構築)
documentation_of: ../../../../../../src/algorithm/geometry/3d/index_set/polyhedron/convex_polyhedron_query_hierarchy_3d_build_support_hierarchy.hpp
---

三次元支持点query用のpolar normal fan階層を構築する内部member定義。

## API

- `ConvexPolyhedronQueryHierarchy3D::build_support_hierarchy()`: 共面三角形をfacetへ統合し、facet法線の球面三角形分割と縮約階層を構築するprivate member。

## API別の時間計算量・空間計算量

頂点数を $V$、面数を $F$ として時間 $O((V+F)\log(V+F))$、保持領域 $O(V+F)$。

## 注意点

private memberの実装leafである。facet incidenceやpolar cellが閉凸多面体の不変条件を満たさない場合は `std::domain_error` または `std::logic_error` を送出する。
