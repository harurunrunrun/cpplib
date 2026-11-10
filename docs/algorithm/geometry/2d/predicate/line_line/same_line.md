---
title: 2D Coincident Lines (二次元同一直線判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/line_line/same_line.hpp
---

## API

- `bool same_line(first, second)`: 2 本の無限直線が一致するか判定する。

## API別の時間計算量・空間計算量

- `same_line`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 2 直線が平行で、かつ `second.a` が `first` 上にあるとき `true` を返す。
- 方向ベクトルの長さが `GEOMETRY_EPS` 以下の退化直線を渡すと、内部の平行判定が `std::invalid_argument` を送出する。
