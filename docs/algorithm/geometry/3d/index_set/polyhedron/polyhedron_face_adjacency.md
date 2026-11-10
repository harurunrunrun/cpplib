---
title: General Polyhedron Face Adjacency (一般多面体の面隣接)
documentation_of: ../../../../../../src/algorithm/geometry/3d/index_set/polyhedron/polyhedron_face_adjacency.hpp
---

## API

- `polyhedron_face_adjacency(polyhedron)`: 各三角形の各辺を共有する隣接三角形の添字を返す。`result[f][i]` は辺 `faces[f][i]`–`faces[f][(i + 1) % 3]` を共有する面であり、境界辺には番兵として面数 `faces.size()` を入れる。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として、基数ソートを用いるため時間 $O(V+F)$、追加領域 $O(V+F)$。

## 注意点

凸性は要求しない。三角形分割上の隣接を返すため、同一平面上の対角線も隣接辺として扱う。3 面以上が共有する辺には `std::domain_error` を送出する。
入力は `geometry3d_validate` で検査し、非有限な頂点や同じ頂点を繰り返す面には `std::invalid_argument`、範囲外の頂点添字には `std::out_of_range` を送出する。
