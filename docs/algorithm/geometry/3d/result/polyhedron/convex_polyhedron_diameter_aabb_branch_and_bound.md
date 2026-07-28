---
title: Convex Polyhedron Diameter by AABB Branch and Bound (AABB分枝限定による凸多面体直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron/convex_polyhedron_diameter_aabb_branch_and_bound.hpp
---

## API

- `convex_polyhedron_diameter_aabb_branch_and_bound(polyhedron)`: 頂点集合のAABB木を構築し、現在の直径を超えられない部分木対を除外して正確な直径を返す。頂点が2個未満なら `std::nullopt`。

## API別の時間計算量・空間計算量

頂点数を $V$、訪れたAABBノード対数を $R$、葉で比較した頂点対数を $C$ とする。平均前処理 $O(V\log V)$、探索 $O(R+C)$、最悪時間 $O(V^2)$、追加領域 $O(V)$。

## 注意点

近似ではなく全頂点対走査版と同じ結果を返す。同距離なら添字対が辞書順最小のものを返す。非有限頂点には `std::invalid_argument` を送出する。
