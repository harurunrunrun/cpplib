---
title: Convex Polyhedron Face Adjacency (凸多面体の面隣接)
documentation_of: ../src/algorithm/geometry/3d/convex_polyhedron_face_adjacency.hpp
---

## API

- `convex_polyhedron_face_adjacency(polyhedron)`: 各三角形の有向3辺の反対側にある面添字を返す。境界辺は `faces.size()`。

## API別の時間計算量・空間計算量

- 面数を $F$ として時間 $O(F\log F)$、領域 $O(F)$。

## 注意点

範囲外添字を拒否し、1辺を3面以上が共有する非多様体には `std::domain_error` を送出する。
