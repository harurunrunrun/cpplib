---
title: Line-Point Distance (直線と点の距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/line_point/line_point_distance.hpp
---

直線（無限に延びる直線）と点の最短距離を求める。

## API

```cpp
long double distance(const Line& line, const Point& point);
```

`line` と `point` の距離を返す。

## 時間計算量

時間計算量 $O(1)$、追加領域 $O(1)$。

## 注意点

`line.a` と `line.b` が（`GEOMETRY_EPS` の許容誤差で）一致する退化した直線には `std::invalid_argument` を送出する。
