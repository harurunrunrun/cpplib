---
title: Distance between 3D Ray and Line in Ray-Line Order (半直線・直線順の三次元距離)
documentation_of: ../../../../src/algorithm/geometry/3d/line_ray_distance_ray_line.hpp
---

## API

- `distance(ray, line)`: `Ray3` と `Line3` の集合間距離を返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

逆順leafは `distance(line, ray)` へ委譲する。返り値と例外条件は順方向leafと同じである。
