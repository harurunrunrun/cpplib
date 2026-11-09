---
title: 2D Dot Product (二次元内積)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_point/dot.hpp
---

2 次元ベクトルの内積を求める。

## API

```cpp
long double dot(const Point& left, const Point& right);
```

## 時間計算量

時間計算量 $O(1)$、追加領域 $O(1)$。

## 注意点

結果は `long double` で計算するため、座標の積が表現可能な範囲に収まる必要がある。
