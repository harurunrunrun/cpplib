---
title: Convex Polyhedron Query Centered Direction (凸多面体クエリ用中心化方向)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron/convex_polyhedron_query_hierarchy_3d_centered_direction.hpp
---

包含階層の放射方向をexact dyadic式で作る内部member定義。

## API

- `ConvexPolyhedronQueryHierarchy3D::centered_direction(point)`: 頂点重心を分母なしで引いた方向を返すprivate member。構築処理から呼ばれる。

## API別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。各座標について定数回の多倍長dyadic演算を行うため、bit計算量は入力指数差に依存する。

## 注意点

private memberの実装leafであり、利用側が直接呼び出すAPIではない。通常は後方互換aggregatorをincludeして `ConvexPolyhedronQueryHierarchy3D` を構築する。
