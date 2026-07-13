---
title: 2D Circle-Circle Intersection Test (二次元円同士の交差判定)
documentation_of: ../src/algorithm/geometry/2d/intersect_circle_circle.hpp
---

## API

- `intersect_circle_circle(first, second)`: 2円周が少なくとも1点を共有するか判定する。

## API別の時間計算量・空間計算量

- `intersect_circle_circle`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

接円と一致する円も真。負半径には `std::invalid_argument` を送出する。
