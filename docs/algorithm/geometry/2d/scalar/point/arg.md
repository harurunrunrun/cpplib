---
title: 2D Vector Argument (二次元ベクトルの偏角)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point/arg.hpp
---

2 次元ベクトルの偏角を求める。

## API

```cpp
long double arg(const Point& point);
```

`std::atan2(point.y, point.x)` による偏角を、ラジアン単位で $[-\pi, \pi]$ の範囲で返す。

## 時間計算量

時間計算量 $O(1)$、追加領域 $O(1)$。

## 注意点

零ベクトルに対する値は、実行環境の `atan2(0, 0)` の挙動に従う。
