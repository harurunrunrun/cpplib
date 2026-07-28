---
title: 3D Ray-Segment Skewness (三次元半直線と線分のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/ray_segment_skew.hpp
---

## API

- `skew(ray, segment)`: `Ray3` と `Segment3` の支持直線同士がねじれの位置にあるなら `true` を返す。
- `skew(segment, ray)`: 引数順を逆にした同じ判定を行う。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

半直線・線分の範囲制限は考慮しない。退化半直線または退化線分なら `std::invalid_argument` を送出する。
