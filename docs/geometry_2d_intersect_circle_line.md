---
title: 2D Circle-Line Intersection Test (二次元円と直線の交差判定)
documentation_of: ../src/algorithm/geometry/2d/intersect_circle_line.hpp
---

## API

- `intersect_circle_line(circle, line)`: 円周と無限直線が共通点を持つか判定する。

## API別の時間計算量・空間計算量

- `intersect_circle_line`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

接する場合も真。負半径または退化直線には `std::invalid_argument` を送出する。
