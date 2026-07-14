---
title: Point Set-Segment Minimum Distance (点集合と線分の最小距離)
documentation_of: ../src/algorithm/geometry/2d/point_set_segment_minimum_distance.hpp
---

## API

`long double point_set_segment_minimum_distance(points, segment)`

## 引数

点集合と線分を渡す。

## 戻り値

各点と線分の距離の最小値。空集合では正の無限大を返す。

## API別の時間計算量・空間計算量

- 点数を $N$ として時間 $O(N)$、追加領域 $O(1)$。

## 注意点

退化線分は1点として扱う。
