---
title: 3D Linear Parallel Test (三次元線形図形の平行判定)
documentation_of: ../src/algorithm/geometry/3d/predicate/linear_parallel.hpp
---

## API

- `parallel(first, second)`: `Line3`, `Ray3`, `Segment3` の全順序付き組合せについて方向が平行かを返す。

## API別の時間計算量・空間計算量

- 全overload: 時間・追加領域 $O(1)$。

## 注意点

いずれかが退化して方向を持たない場合は `std::invalid_argument` を送出する。同一直線上かどうかは判定しない。
