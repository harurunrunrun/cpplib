---
title: Polygon3 Triangulation Triangles (三次元多角形のTriangle3三角形分割)
documentation_of: ../../../../../../src/algorithm/geometry/3d/triangle/polygon/polygon3_triangulation_triangles.hpp
---

## API

- `polygon3_triangulation(polygon)`: 多角形を三角形分割し、各三角形を元の頂点からなる `Triangle3` の列（`std::vector<Triangle3>`）として返す。

## API別の時間計算量・空間計算量

頂点数を $N$ とする。

- `polygon3_triangulation`: 時間 $O(N^2)$、結果を除く追加領域 $O(N)$。返す列の長さは $O(N)$。

## 注意点

- 頂点順に境界をたどる、自己交差がなく正の面積を持つ同一平面上の多角形を与える。不正な入力には `std::invalid_argument` を送出する。
- 内部では `polygon3_triangulation_indices` で頂点インデックスの組を求めてから `Triangle3` に変換している。頂点インデックスが必要な場合は `polygon3_triangulation_indices.hpp` を使う。
