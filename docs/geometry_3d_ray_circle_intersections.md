---
title: 3D Ray and Circle Intersections (三次元半直線と円の交点)
documentation_of: ../src/algorithm/geometry/3d/point_collection/ray_circle_intersections.hpp
---

## API

- `ray_circle_intersections(ray, circle)`: 閉半直線と円周の相異なる交点を0個から2個返す。

## API別の時間計算量・空間計算量

- 時間・出力以外の追加領域 $O(1)$。

## 注意点

計算結果が long double で表現できない場合は std::overflow_error を送出する。交差計算は共通移動量を保つ正規化で中間 overflow を避ける。
Circle3 は中心・法線・半径が有限、法線が非零、半径が非負でなければならず、違反時は std::invalid_argument を送出する。

退化半直線、負半径、零法線には `std::invalid_argument` を送出する。始点も半直線に含む。
