---
title: 2D Line Intersection (二次元直線交点)
documentation_of: ../src/algorithm/geometry/2d/line_intersection.hpp
---

## API

- `line_intersection(first, second)`: `cross_point(first, second)` と同じく、2無限直線の一意な交点を返す。

## API別の時間計算量・空間計算量

- `line_intersection`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化直線には `std::invalid_argument`、平行または一致する直線には `std::domain_error` を送出する。
