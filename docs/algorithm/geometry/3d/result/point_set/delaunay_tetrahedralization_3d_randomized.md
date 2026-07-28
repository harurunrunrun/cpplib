---
title: Randomized 3D Delaunay Tetrahedralization (乱択三次元Delaunay四面体分割)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/delaunay_tetrahedralization_3d_randomized.hpp
---

## API

- `delaunay_tetrahedralization_3d_randomized(points, seed)`: seedで挿入順をshuffleし、`DelaunayTetrahedralization3` を返す。同じ入力とseedから同じ結果を得る。

## API別の時間計算量・空間計算量

一般位置かつ入力独立な乱択順では、四面体・conflict総数を $K$ として期待時間 $O(N\log N+K)$、追加領域 $O(N+K)$。三次元では $K=O(N^2)$ になり得る。

## 注意点

期待計算量が必要なら入力から独立なseedを用いる。座標は有限でなければならず、重複判定は完全一致で行う。
