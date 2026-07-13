---
title: 2D Circle Intersection Area (二次元円盤共通部分面積)
documentation_of: ../src/algorithm/geometry/2d/circle_circle_intersection_area.hpp
---

## API

- `circle_circle_intersection_area(first, second)`: 2円盤の共通部分の面積を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

離れている場合は0、包含時は小さい円盤の面積を返す。負半径には `std::invalid_argument` を送出する。
