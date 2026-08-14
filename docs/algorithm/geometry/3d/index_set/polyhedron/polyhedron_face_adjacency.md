---
title: General Polyhedron Face Adjacency (一般多面体の面隣接)
documentation_of: ../../../../../../src/algorithm/geometry/3d/index_set/polyhedron/polyhedron_face_adjacency.hpp
---

## API

- `polyhedron_face_adjacency(polyhedron)`: 各三角形の各有向辺を共有する隣接三角形indexを返す。境界辺には面数をsentinelとして格納する。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間計算量 $O(V+F)$、追加空間計算量 $O(V+F)$。

## 注意点

凸性は要求しない。三角形分割上の隣接を返すため、同一平面上の対角線も隣接辺として扱う。3面以上が共有する辺には `std::domain_error` を送出する。
