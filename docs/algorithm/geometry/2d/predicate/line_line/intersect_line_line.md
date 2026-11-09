---
title: 2D Line Intersection Test (二次元直線の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/line_line/intersect_line_line.hpp
---

## API

- `bool intersect_line_line(first, second)`: 2 本の無限直線が少なくとも 1 点を共有するか判定する。

## API別の時間計算量・空間計算量

- `intersect_line_line`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 平行でなければ `true`、平行なら一致する場合に限り `true` を返す。
- 方向ベクトルの長さが `GEOMETRY_EPS` 以下の退化直線を渡すと、内部の平行判定が `std::invalid_argument` を送出する。
