---
title: 2D Circle-Circle Cross Points (二次元円同士の交点) [CGL_7_E]
documentation_of: ../src/algorithm/geometry/2d/circle_circle_cross_points.hpp
---

## API

- `circle_circle_cross_points(first, second)`: 2円周の異なる交点を0～2個返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

一致する円は交点が無限にあるため `std::domain_error`、負半径には `std::invalid_argument` を送出する。接点は1個だけ返す。
