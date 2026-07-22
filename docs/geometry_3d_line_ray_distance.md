---
title: Distance between 3D Line and Ray (三次元直線と半直線の距離)
documentation_of: ../src/algorithm/geometry/3d/scalar/line_ray_distance.hpp
---

## API

- `distance(first, second)`: `Line3` と `Ray3` の集合間距離を返す。異種型では逆順overloadも提供する。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

交差または重なる場合は0を返す。退化した直線・半直線・非有限入力には
`std::invalid_argument` を送出する。最近点対を局所座標で求め、距離を表現できない
場合は `std::overflow_error` を送出する。
