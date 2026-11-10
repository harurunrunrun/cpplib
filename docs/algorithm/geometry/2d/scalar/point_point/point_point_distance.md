---
title: Point-Point Distance (点間距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_point/point_point_distance.hpp
---

2 点間のユークリッド距離を求める。

## API

```cpp
long double distance(const Point& first, const Point& second);
```

点 `first` と `second` の距離 `abs(first - second)` を返す。

## 時間計算量

時間計算量 $O(1)$、追加領域 $O(1)$。

## 注意点

戻り値の型は `long double` である。
