---
title: Convex Polyhedron Sphere Intersects (凸多面体と球の交差判定)
documentation_of: ../src/algorithm/geometry/3d/predicate/convex_polyhedron_sphere_intersects.hpp
---

## API

- `convex_polyhedron_sphere_intersects(polyhedron, sphere)`: 球の内部または表面と閉凸形状が共通点を持つか返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間 $O(V+F)$、追加領域 $O(V)$。

## 注意点

Sphere3 は中心・半径が有限かつ半径が非負でなければならず、違反時は std::invalid_argument を送出する。球の中心を原点とする局所座標へ正規化してから距離を計算するため、巨大な共通移動量による中間 overflow を避ける。

負または非有限半径を拒否する。接触はtrue。
