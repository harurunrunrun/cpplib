---
title: Distance between 3D Plane and Line in Plane-Line Order (平面・直線順の三次元距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/plane_line/line_plane_distance_plane_line.hpp
---

## API

- `distance(plane, line)`: `Plane3` と `Line3` の集合間距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 引数の順序を入れ替えて `distance(line, plane)` に委譲する。戻り値と例外の条件は `distance(line, plane)` と同じである。
