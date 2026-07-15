---
title: 3D Skew Linear Primitives (三次元線形図形のねじれ判定)
documentation_of: ../src/algorithm/geometry/3d/linear_skew.hpp
---

## API

- `skew(first, second)`: `Line3`, `Ray3`, `Segment3` の支持直線同士が非平行かつ交わらないかを返す。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

有限範囲が交わらないだけではねじれとしない。線分を含むoverloadで退化線分を渡すと `std::invalid_argument` を送出する。
