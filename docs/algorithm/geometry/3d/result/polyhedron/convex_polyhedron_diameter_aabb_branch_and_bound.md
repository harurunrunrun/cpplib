---
title: Convex Polyhedron Diameter by AABB Branch and Bound (AABB分枝限定による凸多面体直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron/convex_polyhedron_diameter_aabb_branch_and_bound.hpp
---

凸多面体の頂点集合に対して AABB 分枝限定法を適用し、直径を求める。

## API

- `convex_polyhedron_diameter_aabb_branch_and_bound(polyhedron)`: 頂点集合の AABB 木を構築し、現在の直径を超えられない部分木の対を除外して、厳密な直径を `std::optional<PointSetDiameter3DResult>` で返す。頂点が 2 個未満なら `std::nullopt` を返す。

## API別の時間計算量・空間計算量

頂点数を $V$、訪れた AABB ノード対の数を $R$、葉で比較した頂点対の数を $C$ とする。前処理は平均 $O(V \log V)$、探索は $O(R + C)$、最悪時間は $O(V^2)$、追加空間は $O(V)$。

## 注意点

- 近似ではなく、全頂点対を調べた場合と同じ結果を返す。同距離の対が複数あれば、添字対が辞書順最小のものを返す。
- 非有限な頂点には `std::invalid_argument` を送出する。
