---
title: Closest Points for 3D Segment and Plane in Segment-Plane Order (線分・平面順の三次元最近点対)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/segment_plane/segment_plane_closest_points_segment_plane.hpp
---

## API

- `closest_points(segment, plane)`: `Segment3` 上の点と `Plane3` 上の点を引数順のpairで返す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

線分は退化してもよい。零法線平面・非有限入力には `std::invalid_argument` を送出する。交差時は同一点のpair、非交差時は平面に近い端点とその射影を返す。
