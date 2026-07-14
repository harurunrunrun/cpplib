---
title: 2D Distance (二次元距離) [CGL_2_D]
documentation_of: ../src/algorithm/geometry/2d/distance.hpp
---

## API

- `distance(first_point, second_point)`: 点間距離。
- `distance(line, point)`: 無限直線と点の距離。
- `distance_segment_point(segment, point)`: 閉線分と点の距離。
- `distance(first_segment, second_segment)`: 閉線分間距離。
- `distance_line_line(first, second)`: 無限直線間距離。
- `distance_line_segment(line, segment)`: 無限直線と閉線分の距離。

## API別の時間計算量・空間計算量

- 全overload: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

線分は退化してもよい。無限直線を取るAPIへ退化直線を渡すと `std::invalid_argument` を送出する。
`distance_segment_point` は単位方向への射影で端点領域を分類するため、線分長を係数として持つ生の内積への絶対誤差に依存しない。
無限直線の表現は、退化判定に該当しない正の方向scaleに対して同じ距離を返す。
