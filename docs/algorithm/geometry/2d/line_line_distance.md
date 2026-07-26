---
title: Line-Line Distance (直線同士の距離)
documentation_of: ../../../../src/algorithm/geometry/2d/line_line_distance.hpp
---

2本の無限直線の最短距離を求める。

## API

- `distance_line_line(first, second)`: 無限直線 `first` と `second` の距離を返す。

## API別の時間計算量・空間計算量

- `distance_line_line`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

交差または一致する直線には0を返す。退化直線には `std::invalid_argument` を送出する。
