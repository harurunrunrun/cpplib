---
title: 2D Circle-Line Cross Points (二次元円と直線の交点)
documentation_of: ../src/algorithm/geometry/2d/circle_line_cross_points.hpp
---

## API

- `circle_line_cross_points(circle, line)`: 円周と無限直線の異なる交点を0～2個返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

接点は1個だけ返す。負半径または退化直線には `std::invalid_argument` を送出する。
