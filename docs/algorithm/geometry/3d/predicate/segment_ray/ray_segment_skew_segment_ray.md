---
title: 3D Segment-Ray Skewness (三次元線分と半直線のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/segment_ray/ray_segment_skew_segment_ray.hpp
---

## API

- `skew(first, second)`: `Segment3` と `Ray3` の支持直線がねじれの位置にあるとき `true` を返す。

## API別の時間計算量・空間計算量

- `skew`: 時間・追加領域 $O(1)$。

## 注意点

平行な場合と同一平面上で交差しない場合は `false`。入力順は線分、半直線である。
