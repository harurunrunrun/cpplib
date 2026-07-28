---
title: Line-Point Distance (直線と点の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/line_point/line_point_distance.hpp
---

## API

- `distance(line, point)`: 直線と点の最短距離を返す。

## API別の時間計算量・空間計算量

- `distance`: 時間・追加領域 $O(1)$。

## 注意点

直線は退化しておらず、座標は有限値でなければならない。
