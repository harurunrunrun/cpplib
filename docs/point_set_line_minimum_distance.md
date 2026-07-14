---
title: Point Set-Line Minimum Distance (点集合と直線の最小距離)
documentation_of: ../src/algorithm/geometry/2d/point_set_line_minimum_distance.hpp
---

## API

`long double point_set_line_minimum_distance(points, line)`

## 引数

`points` は点集合、`line` は非退化直線である。

## 戻り値

各点と直線の距離の最小値。空集合では正の無限大を返す。

## API別の時間計算量・空間計算量

- 点数を $N$ として時間 $O(N)$、追加領域 $O(1)$。

## 注意点

退化直線では、点集合が空でも `std::invalid_argument` を送出する。
