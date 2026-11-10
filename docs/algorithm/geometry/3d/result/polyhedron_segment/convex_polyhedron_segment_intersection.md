---
title: Convex Polyhedron Segment Intersection (凸多面体と線分の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron_segment/convex_polyhedron_segment_intersection.hpp
---

閉凸多面体と線分の共通部分を求める。

## API

- `convex_polyhedron_segment_intersection(polyhedron, segment)`: 線分と立体の共通部分を `std::optional<Segment3>` で返す。接するだけの場合は両端が等しい線分で、交差しない場合は `std::nullopt` で表す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ とすると、時間 $O(V+F)$、追加空間 $O(1)$。

## 注意点

- 両端点が完全に一致する退化線分は、点の包含判定として扱う。
- 退化していない線分に対しては、多面体が `affine_dimension == 3` であることを要求する。低次元の多面体、非有限な頂点・線分、長さがほぼ零の線分には `std::invalid_argument` を、範囲外の面添字には `std::out_of_range` を送出する。
