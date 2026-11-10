---
title: 3D Segment-Ray Skewness (三次元線分と半直線のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/segment_ray/ray_segment_skew_segment_ray.hpp
---

## API

- `skew(first, second)`: 線分 `first`（`Segment3`）と半直線 `second`（`Ray3`）の支持直線同士がねじれの位置にあるとき `true` を返す。

## API別の時間計算量・空間計算量

- `skew`: 時間・追加領域ともに $O(1)$。

## 注意点

- 線分・半直線の範囲は考慮しない。平行な場合と、支持直線同士が交わる場合は `false` を返す。
- 引数の順序は線分、半直線である。
- 退化した線分・半直線には `std::invalid_argument` を送出する。
