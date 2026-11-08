---
title: Distance between 3D Line and Ray in Line-Ray Order (直線・半直線順の三次元距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/line_ray/line_ray_distance_line_ray.hpp
---

## API

- `distance(line, ray)`: `Line3` と `Ray3` の集合間距離を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 交差または重なる場合は $0$ を返す。
- 退化した直線・半直線、非有限な入力には `std::invalid_argument` を、距離が表現できない場合は `std::overflow_error` を送出する。
