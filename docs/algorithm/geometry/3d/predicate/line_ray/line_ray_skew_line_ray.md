---
title: 3D Line-Ray Skewness (三次元直線と半直線のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_ray/line_ray_skew_line_ray.hpp
---

## API

- `skew(first, second)`: `Line3` と `Ray3` の支持直線がねじれの位置にあるとき `true` を返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 平行な場合と、同一平面上にあって支持直線が交わる場合は `false` を返す。半直線そのものではなく支持直線で判定するため、支持直線が交わっていれば、半直線とは交わらなくても `false` になる。
- 入力は各幾何型の有効条件を満たさなければならない。退化した直線・半直線には `std::invalid_argument` を送出する。
