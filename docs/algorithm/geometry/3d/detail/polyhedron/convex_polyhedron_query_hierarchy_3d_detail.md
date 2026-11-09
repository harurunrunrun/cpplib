---
title: Convex Polyhedron Query Hierarchy Internal Detail (凸多面体クエリ階層内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/polyhedron/convex_polyhedron_query_hierarchy_3d_detail.hpp
---

正確な述語、放射状の階層、AABB 階層、および `ConvexPolyhedronQueryHierarchy3D` の共有本体を実装する。

## API

- `ConvexPolyhedronQueryHierarchy3D(polyhedron, leaf_capacity = 8)`: 検証済みの多面体を所有し、支持点・包含・最近点のクエリのための階層を構築する。`leaf_capacity` が 0 なら `std::invalid_argument` を送出する。
- `support_point(direction)`, `contains(point)`, `closest_point(point)`: 共有本体に定義された公開クエリ。`contains` は境界上の点も含むとみなす。
- `polyhedron()`, `vertex_count()`, `face_count()`, `support_hierarchy_depth()`, `contains_hierarchy_depth()`, `hierarchy_maximum_branching()`: 保持している多面体と階層の情報を返す。
- `segment_intersection(segment)`: 宣言のみで、定義は個別のメンバ実装ヘッダにある。
- `convex_polyhedron_query_hierarchy_3d_detail` 内の型・関数: 正確な演算と階層構築のための内部要素。

## API別の時間計算量・空間計算量

頂点数を $V$、面数を $F$、$N=V+F$ とする。構築は時間 $O(N\log N)$、領域 $O(N)$。三次元の場合、支持点は $O(\log F)$、包含は $O(\log V)$、最近点は訪問する AABB の面ノード数を $K_F$ として $O(\log V+K_F)$、最悪 $O(F)$。

## 注意点

直接 include するための安定した API ではなく、5 つのメンバ実装ヘッダ（`build_membership_hierarchy`, `build_support_hierarchy`, `centered_direction`, `logarithmic_support_index`, `segment_intersection`）から共有される。利用側は集約ヘッダ `convex_polyhedron_query_hierarchy_3d.hpp` を include すること。
座標とクエリは有限でなければならず（違反時は `std::invalid_argument`）、方向は零ベクトルであってはならない。三次元の入力は、閉じていて向きが一貫した凸メッシュでなければならない。
