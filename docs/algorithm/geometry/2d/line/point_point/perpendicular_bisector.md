---
title: Perpendicular Bisector (垂直二等分線)
documentation_of: ../../../../../../src/algorithm/geometry/2d/line/point_point/perpendicular_bisector.hpp
---

## API

`Line perpendicular_bisector(const Point& first, const Point& second)`

## 引数

相異なる 2 点を渡す。

## 戻り値

中点を `a` とし、`a` から `second - first` を反時計回りに 90 度回転した方向へ進んだ点を `b` とする直線。

## API別の時間計算量・空間計算量

- `perpendicular_bisector`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

2 点間の距離が `GEOMETRY_EPS` 以下（許容誤差の範囲で一致）の場合は `std::invalid_argument` を送出する。
