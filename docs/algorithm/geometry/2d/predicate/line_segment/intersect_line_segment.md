---
title: 2D Line-Segment Intersection Test (二次元直線と線分の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/line_segment/intersect_line_segment.hpp
---

## API

- `bool intersect_line_segment(line, segment)`: 無限直線と閉線分が共通点を持つか判定する。

## API別の時間計算量・空間計算量

- `intersect_line_segment`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 線分が直線上に重なる場合も `true` を返す。
- 方向ベクトルの長さが `GEOMETRY_EPS` 以下の退化直線には `std::invalid_argument` を送出する。退化線分は 1 点として扱う。
- 方向ベクトルを単位化して判定するため、退化判定に該当しない範囲では、直線の方向ベクトルを正の定数倍しても判定は変わらない。
