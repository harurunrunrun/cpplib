---
title: 3D Line-Segment Skewness (三次元直線と線分のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/line_segment_skew.hpp
---

## API

- `skew(line, segment)`: `Line3` と `Segment3` の支持直線同士がねじれの位置にあるなら `true` を返す。
- `skew(segment, line)`: 引数の順序を入れ替えた同じ判定を行う。

## API 別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 線分の有限な範囲は考慮しない（支持直線同士で判定する）。
- 退化した直線または退化した線分には `std::invalid_argument` を送出する。
