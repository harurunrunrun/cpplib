---
title: 2D Vector Length (二次元ベクトル長)
documentation_of: ../src/algorithm/geometry/2d/scalar/abs.hpp
---

## API

- `abs(point)`: ベクトルのユークリッド長を返す。

## API別の時間計算量・空間計算量

- `abs`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

`std::hypot(point.x, point.y)` を用いるため、各成分が有限でベクトル長も表現可能なら、成分の二乗がoverflowする大きさでも有限な長さを返す。戻り値は `long double`。
