---
title: 2D Line Intersection Test (二次元直線交差判定)
documentation_of: ../src/algorithm/geometry/2d/predicate/intersect_line_line.hpp
---

## API

- `intersect_line_line(first, second)`: 2無限直線が少なくとも1点を共有するか判定する。

## API別の時間計算量・空間計算量

- `intersect_line_line`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

一致する直線も真を返す。退化直線は渡してはならない。
