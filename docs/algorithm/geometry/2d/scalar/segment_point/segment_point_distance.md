---
title: Segment-Point Distance (線分と点の距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/segment_point/segment_point_distance.hpp
---

閉線分と点の最短距離を求める。

## API

```cpp
long double distance_segment_point(const Segment& segment, const Point& point);
```

`segment` と `point` の距離を返す。

## 時間計算量・空間計算量

時間 $O(1)$、追加空間 $O(1)$。

## 注意点

長さ $0$（許容誤差以下）の退化線分では、端点 `segment.a` と点の距離を返す。
