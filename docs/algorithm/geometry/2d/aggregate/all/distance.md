---
title: 2D Distance (二次元距離) [CGL_2_D]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/distance.hpp
---

点・直線・線分の間の距離を求める API をまとめた集約ヘッダである。

## API

- `distance(first_point, second_point)`: 2 点間の距離。
- `distance(line, point)`: 無限直線と点の距離。
- `distance_segment_point(segment, point)`: 閉線分と点の距離。
- `distance(first_segment, second_segment)`: 2 つの閉線分の距離。
- `distance_line_line(first, second)`: 2 本の無限直線の距離。
- `distance_line_segment(line, segment)`: 無限直線と閉線分の距離。

## 時間計算量・空間計算量

すべてのオーバーロードで、時間 $O(1)$、追加の空間 $O(1)$ である。

## 注意点

- 線分は退化（長さ $0$）していてもよい。無限直線を取る API に退化した直線を渡すと `std::invalid_argument` を送出する。
- `distance_segment_point` は単位方向への射影で端点側の領域を分類するため、線分の長さが係数として掛かった生の内積の絶対誤差には依存しない。
- 無限直線の表現は、退化と判定されない正の方向ベクトルの大きさによらず同じ距離を返す。
