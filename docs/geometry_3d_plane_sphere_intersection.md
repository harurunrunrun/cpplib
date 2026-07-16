---
title: 3D Plane-Sphere Intersection (三次元平面と球面の交円)
documentation_of: ../src/algorithm/geometry/3d/plane_sphere_intersection.hpp
---

## API

- `plane_sphere_intersection(plane, sphere)`: 交円を返し、交わらなければ `nullopt`。接する場合は半径0の円を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

計算結果が long double で表現できない場合は std::overflow_error を送出する。交差計算は共通移動量を保つ正規化で中間 overflow を避ける。
Sphere3 は中心・半径が有限かつ半径が非負でなければならず、違反時は std::invalid_argument を送出する。

零法線または負半径には `std::invalid_argument` を送出する。
