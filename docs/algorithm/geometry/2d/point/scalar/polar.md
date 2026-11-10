---
title: 2D Polar Coordinates (二次元極座標)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point/scalar/polar.hpp
---

## API

- `Point polar(long double radius, long double theta)`: 長さ `radius`、偏角 `theta` の点 $(r \cos \theta, r \sin \theta)$ を返す。角度の単位はラジアンである。

## API別の時間計算量・空間計算量

- `polar`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

負の `radius` も検査せず、式どおりに計算する。
