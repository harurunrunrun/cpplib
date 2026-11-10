---
title: Convex Polyhedron Sphere Intersects (凸多面体と球の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/polyhedron_sphere/convex_polyhedron_sphere_intersects.hpp
---

## API

- `convex_polyhedron_sphere_intersects(polyhedron, sphere)`: 球 `sphere` の内部または表面と閉凸形状 `polyhedron` が共有点を持つかを返す。接する場合も `true` を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間 $O(V+F)$、追加領域 $O(V+F)$（正規化した凸形状を複製する）。

## 注意点

- `Sphere3` は中心・半径が有限かつ半径が非負でなければならず、違反時は `std::invalid_argument` を送出する。頂点が空の凸形状にも `std::invalid_argument` を送出する。
- 球の中心を原点とする局所座標に正規化してから距離を計算するため、巨大な共通の平行移動量による中間計算のオーバーフローを避けられる。
- 退化次元や面の向きの扱いは `convex_polyhedron_contains` と同じである。
