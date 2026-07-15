---
title: Convex Polyhedron Edges (凸多面体の辺列挙)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_edges.hpp
---

## API

- `convex_polyhedron_edges(polyhedron)`: 頂点添字の昇順ペアとして幾何学的な辺を辞書順で返す。同一平面を三角形分割した内部対角線は除外する。

## API別の時間計算量・空間計算量

- 三角形面数を $F$ として時間 $O(F\log F)$、領域 $O(F)$。

## 注意点

面添字が範囲外なら `std::out_of_range`、3面以上が同じ辺を共有する非多様体なら `std::domain_error` を送出する。
