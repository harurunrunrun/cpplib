---
title: Distance between 3D Ray and Line in Ray-Line Order (半直線・直線順の三次元距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/ray_line/line_ray_distance_ray_line.hpp
---

## API

- `distance(ray, line)`: `Ray3` と `Line3` の集合間距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 引数の順序を入れ替えて `distance(line, ray)` に委譲する。戻り値と例外の条件は `distance(line, ray)` と同じである。
