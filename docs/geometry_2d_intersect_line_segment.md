---
title: 2D Line-Segment Intersection Test (二次元直線と線分の交差判定)
documentation_of: ../src/algorithm/geometry/2d/intersect_line_segment.hpp
---

## API

- `intersect_line_segment(line, segment)`: 無限直線と閉線分が共通点を持つか判定する。

## API別の時間計算量・空間計算量

- `intersect_line_segment`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

線分が直線上に重なる場合も真。退化直線には `std::invalid_argument` を送出し、退化線分は1点として扱う。
