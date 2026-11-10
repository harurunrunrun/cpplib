---
title: 3D Line-Line Coincidence (三次元直線同士の一致判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_line/line_line_coincident.hpp
---

## API

- `coincident(first, second)`: 2 本の `Line3` が点集合として同じ無限直線を表すなら `true` を返す。方向が平行で、`second.a` が `first` 上にあるかで判定する。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 方向の正負は問わない。判定は許容誤差 `GEOMETRY3D_EPS` の範囲で行う。
- いずれかが退化した直線なら `std::invalid_argument` を送出する。
