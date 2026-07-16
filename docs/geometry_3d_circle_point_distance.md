---
title: Point to 3D Circle Distance (点と三次元円の距離)
documentation_of: ../src/algorithm/geometry/3d/circle_point_distance.hpp
---

## API

- `distance(circle, point)`: 点と円周との最短距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

計算結果が long double で表現できない場合は std::overflow_error を送出する。交差計算は共通移動量を保つ正規化で中間 overflow を避ける。
Circle3 は中心・法線・半径が有限、法線が非零、半径が非負でなければならず、違反時は std::invalid_argument を送出する。

円板ではなく円周への距離である。負半径または零法線には `std::invalid_argument` を送出する。
