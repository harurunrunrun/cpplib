---
title: Hierarchical Convex Polyhedron Segment Intersection (階層付き凸多面体と線分の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron_segment/convex_polyhedron_query_hierarchy_3d_segment_intersection.hpp
---

`ConvexPolyhedronQueryHierarchy3D` が持つ面の AABB 木を使い、線分と閉凸多面体の共通部分を求める。AABB のスラブ判定では入力の `long double` 値を厳密な 2 進有理数（dyadic）として比較するため、丸めによる見逃し（偽陰性）は生じない。線分と三角形の交差計算は葉でだけ行う。

## API

- `hierarchy.segment_intersection(segment)`: 共通部分を `std::optional<Segment3>` で返す。接するだけの場合は両端が等しい線分、空集合は `std::nullopt` で表す。結果の向きは入力の `segment.a` から `segment.b` への向きにそろえる。

## API別の時間計算量・空間計算量

頂点数を $V$、三角形面の数を $F$、厳密 AABB 木で訪れるノード数を $K$ とする。

- 三次元の多面体: 両端点の包含判定に $O(\log V)$、交差候補の列挙に $O(K)$ 時間。通常は $K = O(\log F + I)$（$I$ は交差候補の面の数）だが、AABB が重なり合う配置での最悪値は $O(F)$。追加空間は木の深さ分の $O(\log F)$。
- 二次元の多面体: 時間 $O(K)$、追加空間 $O(\log F)$。
- 零次元・一次元の多面体: 時間・追加空間ともに $O(1)$。

階層の構築にかかる時間・空間は `ConvexPolyhedronQueryHierarchy3D` のコンストラクタに含まれる。

## 注意点

- 線分の端点は有限値でなければならず、違反した場合は `std::invalid_argument` を送出する。
- 三次元の入力は、クエリ階層が要求する、閉じていて向きが一貫した凸メッシュでなければならない。
- 面との交点の座標は、`segment_triangle_intersection` と同じ浮動小数点精度で返す。
