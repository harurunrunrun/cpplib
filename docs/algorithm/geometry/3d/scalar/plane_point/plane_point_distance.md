---
title: Plane-Point Distance (平面と点の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/plane_point/plane_point_distance.hpp
---

## API

- `distance(plane, point)`: 平面と点の符号なし距離を返す。

## API別の時間計算量・空間計算量

- `distance`: 時間・追加領域ともに $O(1)$。

## 注意点

- `signed_distance(plane, point)` の絶対値を返すため、前提と例外は `signed_distance` と同じである。
- 平面の法線は零ベクトルであってはならず、入力は有限でなければならない。違反時は `std::invalid_argument` を送出する。
