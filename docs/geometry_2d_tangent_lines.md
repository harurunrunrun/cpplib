---
title: 2D Tangent Lines from Point (二次元点から円への接線)
documentation_of: ../src/algorithm/geometry/2d/tangent_lines.hpp
---

## API

- `tangent_lines(circle, point)`: `point` を通る円の異なる接線を0～2本返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

点が円内なら空、円周上なら1本、外部なら2本。負半径には `std::invalid_argument` を送出する。
