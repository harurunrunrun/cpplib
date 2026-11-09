---
title: 3D Ray-Line Skewness (三次元半直線と直線のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/ray_line/line_ray_skew_ray_line.hpp
---

## API

- `skew(first, second)`: 半直線 `first`（`Ray3`）の支持直線と直線 `second`（`Line3`）がねじれの位置にあるとき `true` を返す。

## API別の時間計算量・空間計算量

- `skew`: 時間・追加領域ともに $O(1)$。

## 注意点

- 半直線の範囲（始点と向き）は考慮しない。平行な場合と、支持直線同士が交わる場合（半直線自体とは交わらない場合も含む）は `false` を返す。
- 引数の順序は半直線、直線である。
- 退化した半直線・直線には `std::invalid_argument` を送出する。
