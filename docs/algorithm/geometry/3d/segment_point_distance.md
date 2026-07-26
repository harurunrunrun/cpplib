---
title: Segment-Point Distance (線分と点の距離)
documentation_of: ../../../../src/algorithm/geometry/3d/segment_point_distance.hpp
---

## API

- `distance(segment, point)`: 閉線分と点の最短距離を返す。

## API別の時間計算量・空間計算量

- `distance`: 時間・追加領域 $O(1)$。

## 注意点

退化線分も一点として扱う。座標は有限値でなければならない。
