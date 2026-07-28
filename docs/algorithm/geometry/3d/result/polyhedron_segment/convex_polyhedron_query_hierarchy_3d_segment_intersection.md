---
title: Hierarchical Convex Polyhedron Segment Intersection (階層付き凸多面体と線分の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron_segment/convex_polyhedron_query_hierarchy_3d_segment_intersection.hpp
---

`ConvexPolyhedronQueryHierarchy3D` のface-AABB木を使い、線分と閉凸多面体の
共通部分を求める。AABB slab判定は入力 `long double` をexact dyadicとして比較し、
丸めによるfalse negativeを作らない。leafでだけ線分と三角形を交差させる。

## API

- `hierarchy.segment_intersection(segment)`: 共通部分を線分として返す。
  接点は両端が等しい線分、空集合は `std::nullopt` で表す。結果の向きは
  入力 `segment.a` から `segment.b` への向きにそろえる。

## API別の時間計算量・空間計算量

三角形面数を $F$、exact AABB木で訪問するnode数を $K$ とする。

- 3次元: 両端の包含判定に $O(\log V)$、交差候補列挙に $O(K)$ 時間。
  通常は $K=O(\log F+I)$（$I$ は交差候補面数）だが、AABBが重なる配置での
  最悪値は $O(F)$。追加領域は木の深さ $O(\log F)$。
- 2次元: $O(K)$ 時間、$O(\log F)$ 追加領域。
- 0・1次元: $O(1)$ 時間・追加領域。

階層の構築時間・領域は `ConvexPolyhedronQueryHierarchy3D` の構築子に含まれる。

## 注意点

線分端点は有限値でなければならず、違反時は `std::invalid_argument`。
3次元入力はquery階層が要求する閉じた一貫orientationの凸meshでなければならない。
faceとの交点座標は既存の `segment_triangle_intersection` と同じ浮動小数点精度で返す。
