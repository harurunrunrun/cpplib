---
title: 2D Vector Length (二次元ベクトル長)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point/abs.hpp
---

2 次元ベクトルのユークリッド長を求める。

## API

```cpp
long double abs(const Point& point);
```

## 時間計算量

時間計算量 $O(1)$、追加領域 $O(1)$。

## 注意点

`std::hypot(point.x, point.y)` を用いるため、各成分が有限でベクトル長も表現可能であれば、成分の 2 乗がオーバーフローする大きさでも有限な長さを返す。
