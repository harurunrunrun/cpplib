---
title: Ray-Point Distance (半直線と点の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/ray_point/ray_point_distance.hpp
---

## API

- `distance(ray, point)`: 半直線と点の最短距離を返す。

## API別の時間計算量・空間計算量

- `distance`: 時間・追加領域 $O(1)$。

## 注意点

半直線は退化しておらず、座標は有限値でなければならない。
