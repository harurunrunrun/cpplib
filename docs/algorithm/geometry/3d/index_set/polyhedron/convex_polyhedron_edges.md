---
title: Convex Polyhedron Edges (凸多面体の辺列挙)
documentation_of: ../../../../../../src/algorithm/geometry/3d/index_set/polyhedron/convex_polyhedron_edges.hpp
---

## API

- `convex_polyhedron_edges(polyhedron)`: 幾何的な辺を、頂点添字の昇順の組 `{min, max}` として辞書順に返す。同一平面上の三角形分割で生じた内部の対角線（両側の三角形が共面である辺）は除外する。

## API別の時間計算量・空間計算量

- 三角形の面数を $F$ として、時間 $O(F\log F)$、領域 $O(F)$。

## 注意点

面の頂点添字が範囲外なら `std::out_of_range`、3 面以上が同じ辺を共有する非多様体なら `std::domain_error` を送出する。共面の判定は正確な `adaptive_orient3d` で行う。
