---
title: 3D Line-Sphere Intersections (三次元直線と球面の交点)
documentation_of: ../src/algorithm/geometry/3d/point_collection/line_sphere_intersections.hpp
---

## API

- `line_sphere_intersections(line, sphere)`: 無限直線と球面の異なる交点を直線方向順に0～2個返す。

## API別の時間計算量・空間計算量

- 時間・出力・追加領域 $O(1)$。

## 注意点

計算結果が long double で表現できない場合は std::overflow_error を送出する。交差計算は共通移動量を保つ正規化で中間 overflow を避ける。
Sphere3 は中心・半径が有限かつ半径が非負でなければならず、違反時は std::invalid_argument を送出する。

接点は1個。退化直線または負半径には `std::invalid_argument` を送出する。
