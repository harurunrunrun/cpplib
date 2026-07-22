---
title: Minimum Distance between Convex Polygons (凸多角形間の最小距離)
documentation_of: ../src/algorithm/geometry/2d/scalar/convex_polygon_minimum_distance.hpp
---

## API

- `convex_polygon_minimum_distance(first, second)`: 2閉凸集合間の最短距離を返す。

## 引数

`first`, `second` は弱凸な周回列である。

## 戻り値

交差・接触する場合は0、それ以外は正のユークリッド距離を返す。

## API別の時間計算量・空間計算量

頂点数を $N,M$ として時間・領域 $O(N+M)$。

## 注意点

空集合を含む場合は `std::invalid_argument` を送出する。
