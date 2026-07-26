---
title: Line-Point Distance (直線と点の距離)
documentation_of: ../../../../src/algorithm/geometry/2d/line_point_distance.hpp
---

無限直線と点の最短距離を求める。

## API

- `distance(line, point)`: `line` と `point` の距離を返す。

## API別の時間計算量・空間計算量

- `distance`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

`line.a == line.b` の退化直線には `std::invalid_argument` を送出する。戻り値の型は `long double` である。
