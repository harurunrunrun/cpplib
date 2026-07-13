---
title: 2D Circle-Segment Intersection Test (二次元円と線分の交差判定)
documentation_of: ../src/algorithm/geometry/2d/intersect_circle_segment.hpp
---

## API

- `intersect_circle_segment(circle, segment)`: 円周と閉線分が共通点を持つか判定する。

## API別の時間計算量・空間計算量

- `intersect_circle_segment`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

接触と端点を含む。負半径には `std::invalid_argument` を送出し、退化線分は1点として扱う。
