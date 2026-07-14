---
title: Convex Polygon Minkowski Difference (凸多角形のミンコフスキー差)
documentation_of: ../src/algorithm/geometry/2d/advanced/convex_polygon_minkowski_difference.hpp
---

## API

- `convex_polygon_minkowski_difference(first, second)`: $\{a-b\mid a\in A,b\in B\}$ を返す。

## 引数

`first`, `second` は弱凸な周回列である。

## 戻り値

差集合を正規化済みの点・線分・反時計回り凸多角形として返す。

## API別の時間計算量・空間計算量

頂点数を $N,M$ として時間・領域 $O(N+M)$。

## 注意点

どちらかが空なら空を返す。非凸入力には `std::invalid_argument` を送出する。
