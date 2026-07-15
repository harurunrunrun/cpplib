---
title: 3D Segment and Circle Intersections (三次元線分と円の交点)
documentation_of: ../src/algorithm/geometry/3d/segment_circle_intersections.hpp
---

## API

- `segment_circle_intersections(segment, circle)`: 閉線分と円周の相異なる交点を0個から2個返す。

## API別の時間計算量・空間計算量

- 時間・出力以外の追加領域 $O(1)$。

## 注意点

退化線分に対応する。負半径または零法線には `std::invalid_argument` を送出する。
