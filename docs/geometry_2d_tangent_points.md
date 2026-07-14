---
title: 2D Tangent Points (二次元円への接点) [CGL_7_F]
documentation_of: ../src/algorithm/geometry/2d/tangent_points.hpp
---

## API

- `tangent_points(circle, point)`: `point` を通る円の接線について、円周上の異なる接点を0～2個返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

点が円内なら空、円周上なら1点、外部なら2点。負半径には `std::invalid_argument` を送出する。
