---
title: 3D Line-Segment Skewness (三次元直線と線分のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_segment/line_segment_skew_line_segment.hpp
---

## API

- `skew(first, second)`: `Line3` と `Segment3` の支持直線がねじれの位置にあるとき `true` を返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 平行な場合と、同一平面上にあって支持直線が交わる場合は `false` を返す。線分そのものではなく支持直線で判定する。
- 入力は各幾何型の有効条件を満たさなければならない。退化した直線・線分には `std::invalid_argument` を送出する。
