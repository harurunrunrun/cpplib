---
title: Brute Force 3D Point Set Diameter (全点対による三次元点集合直径)
documentation_of: ../../../../src/algorithm/geometry/3d/point_set_diameter_3d_default.hpp
---

全点対を比較し、三次元点集合のEuclidean直径を正確に求める。

## API

- `point_set_diameter_3d(points)`: 最大距離と両端indexを
  `PointSetDiameter3DResult` で返す。同距離ならindex対が辞書順最小の
  結果を選び、点が2個未満なら `std::nullopt`。

## API別の時間計算量・空間計算量

点数を $N$ として時間 $O(N^2)$、追加領域 $O(1)$。

## 注意点

座標は有限でなければならず、違反時は `std::invalid_argument`。
距離比較は座標差を仮数と2進指数に分けてoverflowを避ける。真の距離が
`long double` 上限を超える場合、返す `distance` は正の無限大となる。
