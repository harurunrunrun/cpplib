---
title: Ray-Point Closest Point (半直線上の点への最近点)
documentation_of: ../../../../src/algorithm/geometry/3d/ray_point_closest_point.hpp
---

## API

- `closest_point(ray, point)`: 点に最も近い半直線上の点を返す。

## API別の時間計算量・空間計算量

- 対応API: 時間・追加領域 $O(1)$。

## 注意点

半直線は退化しておらず、座標は有限値でなければならない。
