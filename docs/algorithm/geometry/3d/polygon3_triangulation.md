---
title: Polygon3 Triangulation Umbrella (三次元多角形の三角形分割集約)
documentation_of: ../../../../src/algorithm/geometry/3d/polygon3_triangulation.hpp
---

## API

- `polygon3_triangulation_indices(polygon)`: 分割結果を元頂点indexの3要素配列で返す。
- `polygon3_triangulation(polygon)`: 分割結果を `Triangle3` の列で返す。
- このヘッダは2つの返却形式のleafをまとめてincludeする後方互換aggregatorである。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として各APIは時間 $O(N^2)$、結果を除く追加領域 $O(N)$。返却列は $O(N)$。

## 注意点

必要な返却形式だけを使う場合は `polygon3_triangulation_indices.hpp` または `polygon3_triangulation_triangles.hpp` を直接includeできる。
