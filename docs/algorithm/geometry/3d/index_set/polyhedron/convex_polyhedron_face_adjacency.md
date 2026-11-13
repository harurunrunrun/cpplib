---
title: Convex Polyhedron Face Adjacency (凸多面体の面隣接)
documentation_of: ../../../../../../src/algorithm/geometry/3d/index_set/polyhedron/convex_polyhedron_face_adjacency.hpp
---

## API

- `convex_polyhedron_face_adjacency(polyhedron)`: 各三角形の 3 辺について、反対側にある面の添字を返す。`result[f][i]` は辺 `faces[f][i]`–`faces[f][(i + 1) % 3]` を共有する面であり、共有する面がない境界辺には `faces.size()` を入れる。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として、辺を基数ソートでまとめるので時間 $O(V + F)$、領域 $O(V + F)$。

## 注意点

面の頂点添字が範囲外なら `std::out_of_range`、1 辺を 3 面以上が共有する非多様体なら `std::domain_error` を送出する。
