---
title: Closest Points for 3D Plane and Segment in Plane-Segment Order (平面・線分順の三次元最近点対)
documentation_of: ../../../../src/algorithm/geometry/3d/segment_plane_closest_points_plane_segment.hpp
---

## API

- `closest_points(plane, segment)`: `Plane3` 上の点と `Segment3` 上の点を引数順のpairで返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

逆順leafは `closest_points(segment, plane)` の結果を入れ替える。最近点の選択と例外条件は順方向leafと同じである。
