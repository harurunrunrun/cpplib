---
title: Distance between 3D Segment and Ray in Segment-Ray Order (線分・半直線順の三次元距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/segment_ray/ray_segment_distance_segment_ray.hpp
---

## API

- `distance(segment, ray)`: `Segment3` と `Ray3` の集合間の最短距離を `long double` で返す。

## API別の時間計算量・空間計算量

- `distance`: 時間・追加領域 $O(1)$。

## 注意点

引数を逆順にした `distance(ray, segment)` へ委譲するだけの関数であり、返り値と例外条件はそちらと同じである。すなわち、座標が有限でない場合や半直線の `origin` と `through` が一致する場合は `std::invalid_argument` を、距離が `long double` で表現できない場合は `std::overflow_error` を送出する。
