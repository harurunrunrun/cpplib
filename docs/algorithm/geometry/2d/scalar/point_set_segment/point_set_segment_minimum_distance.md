---
title: Point Set-Segment Minimum Distance (点集合と線分の最小距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set_segment/point_set_segment_minimum_distance.hpp
---

点集合の各点と閉線分との距離の最小値を求める。

## API

```cpp
long double point_set_segment_minimum_distance(
    const std::vector<Point>& points,
    const Segment& segment
);
```

## 戻り値

各点と線分の距離（`distance_segment_point`）の最小値。点集合が空なら正の無限大を返す。

## 時間計算量・空間計算量

点数を $N$ として、時間 $O(N)$、追加空間 $O(1)$。

## 注意点

長さ $0$ の退化線分は 1 点として扱う。
