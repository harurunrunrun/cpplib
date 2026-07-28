---
title: 3D Ray-Line Skewness (三次元半直線と直線のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/ray_line/line_ray_skew_ray_line.hpp
---

## API

- `skew(first, second)`: `Ray3` と `Line3` の支持直線がねじれの位置にあるとき `true` を返す。

## API別の時間計算量・空間計算量

- `skew`: 時間・追加領域 $O(1)$。

## 注意点

平行な場合と同一平面上で交差しない場合は `false`。入力順は半直線、直線である。
