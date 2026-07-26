---
title: Polygon3 Triangulation Indices (三次元多角形の頂点番号三角形分割)
documentation_of: ../../../../src/algorithm/geometry/3d/polygon3_triangulation_indices.hpp
---

## API

- `polygon3_triangulation_indices(polygon)`: 各三角形を元の `Polygon3` に対する3頂点indexの配列として返す。頂点数を $N$ とすると結果は $N-2$ 個である。

## API別の時間計算量・空間計算量

- `polygon3_triangulation_indices`: 時間 $O(N^2)$、結果を除く追加領域 $O(N)$。返却列は $O(N)$。

## 注意点

頂点順に境界を表す、自己交差のない正の面積を持つ共面多角形を与える。不正入力には `std::invalid_argument` を送出する。
