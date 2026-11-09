---
title: Convex Polyhedron Support Point (凸多面体の支持点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/polyhedron_point/convex_polyhedron_support_point.hpp
---

## API

- `convex_polyhedron_support_point(polyhedron, direction)`: `dot(point, direction)` を最大化する頂点を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$ として時間 $O(V)$、追加領域 $O(1)$（exact dyadic 演算の桁領域を除く）。

## 注意点

- 頂点が空、`direction` が零ベクトル、`direction` または頂点が非有限の場合は `std::invalid_argument` を送出する。
- 目的値は先頭頂点からの差と `direction` の内積を exact dyadic 演算で求めて比較する。そのため、巨大な共通の平行移動量に比べて局所的な差が非常に小さい場合でも支持点を区別できる。
- 最大となる頂点が複数ある場合は、頂点列で最初のものを返す。
