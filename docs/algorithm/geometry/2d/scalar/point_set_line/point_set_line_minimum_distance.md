---
title: Point Set-Line Minimum Distance (点集合と直線の最小距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set_line/point_set_line_minimum_distance.hpp
---

点集合と直線の間の最小距離を求める。

## API

```cpp
long double point_set_line_minimum_distance(
    const std::vector<Point>& points,
    const Line& line
);
```

## 引数

`points` は点集合、`line` は非退化な直線である。

## 戻り値

各点と直線の距離の最小値。空集合に対しては正の無限大を返す。

## 時間計算量

点数を $N$ として、時間計算量 $O(N)$、追加領域 $O(1)$。

## 注意点

直線が退化している場合は、点集合が空であっても `std::invalid_argument` を送出する。
