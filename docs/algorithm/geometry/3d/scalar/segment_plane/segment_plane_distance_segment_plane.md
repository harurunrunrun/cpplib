---
title: Distance between 3D Segment and Plane in Segment-Plane Order (線分・平面順の三次元距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/segment_plane/segment_plane_distance_segment_plane.hpp
---

## API

- `distance(segment, plane)`: `Segment3` と `Plane3` の集合間距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 交差する、または線分が平面上にある場合は $0$ を返す。交差しない場合は平面に近い方の端点と平面の距離になる。
- 線分は退化（両端点が一致）していてもよい。
- 法線が零ベクトルの平面、非有限な入力には `std::invalid_argument` を送出する。
