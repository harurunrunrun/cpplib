---
title: 2D Intersection Test (二次元交差判定)
documentation_of: ../src/algorithm/geometry/2d/intersect.hpp
---

## API

- `intersect(first_segment, second_segment)`: 2閉線分が共通点を持つか判定する。
- `intersect(line, point)`: 点が無限直線上にあるか判定する。

## API別の時間計算量・空間計算量

- 各overload: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

線分版は端点接触、重なり、退化線分を含む。直線と点の版は退化直線に `std::invalid_argument` を送出する。
