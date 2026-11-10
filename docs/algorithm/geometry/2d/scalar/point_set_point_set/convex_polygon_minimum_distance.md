---
title: Minimum Distance between Convex Polygons (凸多角形間の最小距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set_point_set/convex_polygon_minimum_distance.hpp
---

2 つの凸多角形（閉凸集合）の間の最短距離を求める。

## API

```cpp
long double convex_polygon_minimum_distance(
    std::vector<Point> first,
    std::vector<Point> second
);
```

## 引数

`first`, `second` は凸多角形を周回する弱凸な頂点列である。

## 戻り値

交差または接触する場合は 0、それ以外は正のユークリッド距離を返す。Minkowski 差 `first - second` と原点との距離として計算する。

## 時間計算量

頂点数を $N, M$ として、時間計算量・空間計算量ともに $O(N + M)$。

## 注意点

空集合を含む場合や、入力が凸でない場合は `std::invalid_argument` を送出する。
