---
title: DelaunayTetrahedralization3 (三次元Delaunay四面体分割結果)
documentation_of: ../src/algorithm/geometry/3d/shape/delaunay_tetrahedralization_result3.hpp
---

三次元Delaunay四面体分割の頂点と四面体を保持する結果型。

## API

- `affine_dimension`: 入力点集合のaffine dimension。空集合は `-1`、一点、
  共線、共面、三次元をそれぞれ `0,1,2,3` とする。
- `vertices`: 完全一致する重複を除いた点を辞書順に保持する。
- `tetrahedra`: `vertices` の添字4個からなる正向き四面体を保持する。

## API別の時間計算量・空間計算量

各memberへの参照は時間・追加領域 $O(1)$。格納領域は頂点数を $N$、四面体数を
$T$ として $O(N+T)$。

## 注意点

- `affine_dimension < 3` のとき `tetrahedra` は空になる。
- 四面体の各面の向きではなく、4頂点全体の `orient3d` が正になる順序を保証する。
