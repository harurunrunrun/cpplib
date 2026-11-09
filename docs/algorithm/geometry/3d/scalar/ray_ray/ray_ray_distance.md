---
title: Distance between 3D Ray and Ray (三次元半直線同士の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/ray_ray/ray_ray_distance.hpp
---

## API

- `distance(first, second)`: 2 つの `Ray3` の集合間距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 交差または重なる場合は $0$ を返す。
- 退化した半直線、非有限な入力には `std::invalid_argument` を送出する。
- 最近点対を局所座標で求め、距離を表現できない場合は `std::overflow_error` を送出する。
