---
title: 2D Line Cross Point (二次元直線交点) [CGL_2_C]
documentation_of: ../src/algorithm/geometry/2d/cross_point.hpp
---

## API

- `cross_point(first, second)`: 2無限直線の一意な交点を返す。

## API別の時間計算量・空間計算量

- `cross_point`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化直線には `std::invalid_argument`、平行または一致して一意な交点がない場合には `std::domain_error` を送出する。
