---
title: 3D Delaunay Tetrahedralization with Default Seed (固定seed三次元Delaunay四面体分割)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/delaunay_tetrahedralization_3d_default.hpp
---

## API

- `delaunay_tetrahedralization_3d(points)`: 完全一致する重複点を除き、固定seedのBowyer--Watson法で `DelaunayTetrahedralization3` を返す。同じ入力から同じ四面体列を得る。

## API別の時間計算量・空間計算量

出力と増分中の四面体・conflict総数を $K$、検査したconflict対数を $C$ として時間 $O(N\log N+K+C)$、追加領域 $O(N+K)$。最悪 $K=O(N^2)$、$C=O(NK)$。

## 注意点

座標は有限でなければならない。共線・共面入力では四面体を生成しない。有限なsuper tetraで覆えない入力には `std::overflow_error` を送出する。
