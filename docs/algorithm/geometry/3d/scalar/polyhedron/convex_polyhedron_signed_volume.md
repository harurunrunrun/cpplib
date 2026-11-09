---
title: Convex Polyhedron Signed Volume (凸多面体の向き付き体積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/polyhedron/convex_polyhedron_signed_volume.hpp
---

## API

- `convex_polyhedron_signed_volume(polyhedron)`: 各向き付き三角形面と基準点（先頭頂点）が作る四面体の符号付き体積の和を返す。面が外向きなら正になる。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として、時間 $O(V + F)$、追加領域 $O(1)$。

## 注意点

- 面が閉じていて、向きが一貫していることを前提とする。
- アフィン次元が 3 未満の場合や頂点が空の場合は $0$ を返す。
- 非有限な頂点には `std::invalid_argument` を、範囲外の頂点添字には `std::out_of_range` を、結果のオーバーフローには `std::overflow_error` を送出する。
