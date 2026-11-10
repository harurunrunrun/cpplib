---
title: 3D Line-Line Skewness (三次元直線同士のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_line/line_line_skew.hpp
---

## API

- `skew(first, second)`: 2 本の `Line3` が平行でなく、かつ交点を持たない（ねじれの位置にある）なら `true` を返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 平行な相異なる直線と、一致する直線はいずれも `false` を返す。
- 退化した直線には `std::invalid_argument` を送出する。
