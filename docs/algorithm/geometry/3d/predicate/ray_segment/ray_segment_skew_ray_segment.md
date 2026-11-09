---
title: 3D Ray-Segment Skewness (三次元半直線と線分のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/ray_segment/ray_segment_skew_ray_segment.hpp
---

## API

- `skew(first, second)`: 半直線 `first`（`Ray3`）と線分 `second`（`Segment3`）の支持直線同士がねじれの位置にあるとき `true` を返す。

## API別の時間計算量・空間計算量

- `skew`: 時間・追加領域ともに $O(1)$。

## 注意点

- 半直線・線分の範囲は考慮しない。平行な場合と、支持直線同士が交わる場合は `false` を返す。
- 退化した半直線・線分には `std::invalid_argument` を送出する。
