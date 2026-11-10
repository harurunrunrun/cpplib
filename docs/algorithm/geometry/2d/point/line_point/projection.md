---
title: 2D Projection (二次元射影) [CGL_1_A]
documentation_of: ../../../../../../src/algorithm/geometry/2d/point/line_point/projection.hpp
---

## API

- `Point projection(const Line& line, const Point& point)`: `point` を無限直線 `line` へ正射影した点を返す。

## API別の時間計算量・空間計算量

- `projection`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

`line.a` と `line.b` の距離が `GEOMETRY_EPS` 以下（許容誤差の範囲で一致）の場合は `std::invalid_argument` を送出する。
