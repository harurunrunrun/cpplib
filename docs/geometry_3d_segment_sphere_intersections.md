---
title: 3D Segment-Sphere Intersections (三次元線分と球面の交点)
documentation_of: ../src/algorithm/geometry/3d/segment_sphere_intersections.hpp
---

## API

- `segment_sphere_intersections(segment, sphere)`: 閉線分と球面の異なる交点を0～2個返す。

## API別の時間計算量・空間計算量

- 時間・出力・追加領域 $O(1)$。

## 注意点

計算結果が long double で表現できない場合は std::overflow_error を送出する。交差計算は共通移動量を保つ正規化で中間 overflow を避ける。
Sphere3 は中心・半径が有限かつ半径が非負でなければならず、違反時は std::invalid_argument を送出する。

退化線分を1点として扱う。負半径には `std::invalid_argument` を送出する。
