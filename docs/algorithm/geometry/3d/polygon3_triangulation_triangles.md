---
title: Polygon3 Triangulation Triangles (三次元多角形のTriangle3三角形分割)
documentation_of: ../../../../src/algorithm/geometry/3d/polygon3_triangulation_triangles.hpp
---

## API

- `polygon3_triangulation(polygon)`: 分割後の各領域を、元頂点からなる `Triangle3` の列として返す。

## API別の時間計算量・空間計算量

- `polygon3_triangulation`: 時間 $O(N^2)$、結果を除く追加領域 $O(N)$。返却列は $O(N)$。

## 注意点

頂点順に境界を表す、自己交差のない正の面積を持つ共面多角形を与える。不正入力には `std::invalid_argument` を送出する。頂点indexが必要なら `polygon3_triangulation_indices.hpp` を使う。
