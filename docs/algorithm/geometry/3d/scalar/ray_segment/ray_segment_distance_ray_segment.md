---
title: Distance between 3D Ray and Segment in Ray-Segment Order (半直線・線分順の三次元距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/ray_segment/ray_segment_distance_ray_segment.hpp
---

## API

- `distance(ray, segment)`: `Ray3` と `Segment3` の集合間距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 交差または重なる場合は $0$ を返す。
- 線分は退化（両端点が一致）していてもよい。
- 退化した半直線、非有限な入力には `std::invalid_argument` を、距離が表現できない場合は `std::overflow_error` を送出する。
