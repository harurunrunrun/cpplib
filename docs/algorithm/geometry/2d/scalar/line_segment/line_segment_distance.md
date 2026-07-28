---
title: Line-Segment Distance (直線と線分の距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/line_segment/line_segment_distance.hpp
---

無限直線と閉線分の最短距離を求める。

## API

- `distance_line_segment(line, segment)`: `line` と `segment` の距離を返す。

## API別の時間計算量・空間計算量

- `distance_line_segment`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

交差する場合は0を返す。退化線分を許すが、退化直線には `std::invalid_argument` を送出する。
