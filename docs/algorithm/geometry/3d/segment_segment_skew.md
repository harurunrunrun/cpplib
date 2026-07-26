---
title: 3D Segment-Segment Skewness (三次元線分同士のねじれ判定)
documentation_of: ../../../../src/algorithm/geometry/3d/segment_segment_skew.hpp
---

## API

- `skew(first, second)`: 2本の `Segment3` の支持直線同士がねじれの位置にあるなら `true` を返す。

## API別の時間計算量・空間計算量

- `skew`: 時間・追加領域 $O(1)$。

## 注意点

線分の有限範囲は考慮しない。いずれかが退化線分なら `std::invalid_argument` を送出する。
