---
title: Maximum Distance between Convex Polygons (凸多角形間の最大距離)
documentation_of: ../src/algorithm/geometry/2d/convex_polygon_maximum_distance.hpp
---

## API

- `convex_polygon_maximum_distance(first, second)`: 2閉凸集合から1点ずつ選ぶ距離の最大値を返す。

## 引数

`first`, `second` は弱凸な周回列である。

## 戻り値

最大ユークリッド距離を `long double` で返す。

## API別の時間計算量・空間計算量

頂点数を $N,M$ として時間・領域 $O(N+M)$。

## 注意点

空集合を含む場合は `std::invalid_argument` を送出する。
