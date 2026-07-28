---
title: 3D Line-Line Coincidence (三次元直線同士の一致判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_line/line_line_coincident.hpp
---

## API

- `coincident(first, second)`: 2本の `Line3` が点集合として同じ無限直線を表すなら `true` を返す。

## API別の時間計算量・空間計算量

- `coincident`: 時間・追加領域 $O(1)$。

## 注意点

方向の正負は問わない。いずれかが退化直線なら `std::invalid_argument` を送出する。
