---
title: 3D Line-Ray Skewness (三次元直線と半直線のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/line_ray/line_ray_skew_line_ray.hpp
---

## API

- `skew(first, second)`: `Line3` と `Ray3` の支持直線がねじれの位置にあるとき `true` を返す。

## API別の時間計算量・空間計算量

- `skew`: 時間・追加領域 $O(1)$。

## 注意点

平行な場合と同一平面上で交差しない場合は `false`。入力は各幾何型の有効条件を満たす必要がある。
