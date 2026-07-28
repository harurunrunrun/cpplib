---
title: Point-Point Distance (点間距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_point/point_point_distance.hpp
---

2点間のユークリッド距離を求める。

## API

- `distance(first, second)`: 点 `first` と `second` の距離を返す。

## API別の時間計算量・空間計算量

- `distance`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

戻り値の型は `long double` である。
