---
title: 3D Line-Segment Skewness (三次元直線と線分のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/line_segment_skew.hpp
---

## API

- `skew(line, segment)`: `Line3` と `Segment3` の支持直線同士がねじれの位置にあるなら `true` を返す。
- `skew(segment, line)`: 引数順を逆にした同じ判定を行う。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

線分の有限範囲は考慮しない。退化直線または退化線分なら `std::invalid_argument` を送出する。
