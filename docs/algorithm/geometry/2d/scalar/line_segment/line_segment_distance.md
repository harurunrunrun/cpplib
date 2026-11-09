---
title: Line-Segment Distance (直線と線分の距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/line_segment/line_segment_distance.hpp
---

直線（無限に延びる直線）と閉線分の最短距離を求める。

## API

```cpp
long double distance_line_segment(const Line& line, const Segment& segment);
```

`line` と `segment` の距離を返す。

## 時間計算量

時間計算量 $O(1)$、追加領域 $O(1)$。

## 注意点

交差する場合は 0 を返し、そうでなければ線分の両端点から直線までの距離の小さい方を返す。退化した線分は許すが、退化した直線には `std::invalid_argument` を送出する。
