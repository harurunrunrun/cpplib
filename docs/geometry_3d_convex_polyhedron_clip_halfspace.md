---
title: Convex Polyhedron Halfspace Clip (凸多面体の半空間切断)
documentation_of: ../src/algorithm/geometry/3d/shape/convex_polyhedron_clip_halfspace.hpp
---

## API

- `convex_polyhedron_clip_halfspace(polyhedron, boundary, keep_negative_side=true)`: `dot(normal,p-point)<=0` 側との共通部分を凸包として返す。falseなら反対側を残す。

## API別の時間計算量・空間計算量

- 辺数を $E$、候補点数を $K$ として時間 $O(E\log E+K^2)$、領域 $O(E+K)$。

## 注意点

空の共通部分は `affine_dimension==-1` で返す。境界上の点は両側に含める。
側判定と交点はraw法線のexact dyadic演算で求め、候補点から凸包を再構成する。
ゼロ法線・非有限入力を拒否し、表現不能な交点には `std::overflow_error` を送出する。
