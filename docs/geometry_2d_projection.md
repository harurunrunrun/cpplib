---
title: 2D Projection (二次元射影) [CGL_1_A]
documentation_of: ../src/algorithm/geometry/2d/point/projection.hpp
---

## API

- `projection(line, point)`: `point` を無限直線 `line` へ正射影した点を返す。

## API別の時間計算量・空間計算量

- `projection`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

`line.a` と `line.b` が許容誤差の範囲で一致する場合は `std::invalid_argument` を送出する。
