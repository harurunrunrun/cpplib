---
title: Convex Polyhedron Support Hierarchy Builder (凸多面体支持点階層構築)
documentation_of: ../../../../../../src/algorithm/geometry/3d/index_set/polyhedron/convex_polyhedron_query_hierarchy_3d_build_support_hierarchy.hpp
---

三次元の支持点クエリのための、法線ファン（極双対のセル分割）の階層を構築するメンバ関数の定義である。

## API

- `ConvexPolyhedronQueryHierarchy3D::build_support_hierarchy()`: 共面な三角形をファセットへ統合し、ファセット法線による球面の三角形分割と、その縮約階層を構築する `private` メンバ関数。

## API別の時間計算量・空間計算量

頂点数を $V$、面数を $F$ として、時間 $O((V+F)\log(V+F))$、保持する領域 $O(V+F)$。

## 注意点

`private` メンバ関数を実装する個別ヘッダである。ファセットの接続や極のセルが閉じた凸多面体の不変条件を満たさない場合は、`std::domain_error` または `std::logic_error` を送出する。
