---
title: 2D Squared Norm (二次元ノルム二乗)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point/norm.hpp
---

2 次元ベクトルの長さの 2 乗を求める。

## API

```cpp
long double norm(const Point& point);
```

原点からのユークリッド距離の 2 乗 $x^2 + y^2$ を返す。

## 時間計算量

時間計算量 $O(1)$、追加領域 $O(1)$。

## 注意点

平方根は取らない。長さが必要な場合は `abs(point)` を用いる。
