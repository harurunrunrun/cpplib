---
title: 3D Ray-Ray Skewness (三次元半直線同士のねじれ判定)
documentation_of: ../../../../src/algorithm/geometry/3d/ray_ray_skew.hpp
---

## API

- `skew(first, second)`: 2本の `Ray3` の支持直線同士がねじれの位置にあるなら `true` を返す。

## API別の時間計算量・空間計算量

- `skew`: 時間・追加領域 $O(1)$。

## 注意点

半直線の向きと始点による範囲制限は考慮しない。退化半直線には `std::invalid_argument` を送出する。
