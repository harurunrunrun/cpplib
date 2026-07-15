---
title: Point to 3D Circle Distance (点と三次元円の距離)
documentation_of: ../src/algorithm/geometry/3d/circle_point_distance.hpp
---

## API

- `distance(circle, point)`: 点と円周との最短距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

円板ではなく円周への距離である。負半径または零法線には `std::invalid_argument` を送出する。
