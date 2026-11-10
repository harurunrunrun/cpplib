---
title: General Polyhedron-Sphere Intersection (一般多面体と球の交差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_sphere/polyhedron_sphere_intersects.hpp
---

## API

- `polyhedron_sphere_intersects(polyhedron, sphere)`: 非凸でもよい多面体の閉領域と閉球 `sphere` が共有点を持つかを返す。接する場合も `true` を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間 $O(V+F\log F)$、追加空間 $O(V+F)$。内訳は、中心の包含判定が $O(F)$、AABB 階層の構築が $O(F\log F)$、優先度付きキューによる最近面の探索が最悪 $O(F\log F)$ である。

## 注意点

- 球の中心が多面体に含まれるか、中心から表面までの距離が半径以下なら `true` を返す。
- 負の半径・非有限な球、面のない多面体には `std::invalid_argument` を送出する。多面体の頂点や面についての検査は `polyhedron_contains` と同じである。
