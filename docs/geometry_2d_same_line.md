---
title: 2D Coincident Lines (二次元同一直線判定)
documentation_of: ../src/algorithm/geometry/2d/predicate/same_line.hpp
---

## API

- `same_line(first, second)`: 2無限直線が一致するか判定する。

## API別の時間計算量・空間計算量

- `same_line`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化直線には、内部の直線上判定が `std::invalid_argument` を送出する。
