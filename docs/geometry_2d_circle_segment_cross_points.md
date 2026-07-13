---
title: 2D Circle-Segment Cross Points (二次元円と線分の交点)
documentation_of: ../src/algorithm/geometry/2d/circle_segment_cross_points.hpp
---

## API

- `circle_segment_cross_points(circle, segment)`: 円周と閉線分の異なる交点を0～2個返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

端点の交差を含み、重複点は除く。負半径には `std::invalid_argument` を送出し、退化線分は1点として扱う。
