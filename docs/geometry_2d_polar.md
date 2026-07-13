---
title: 2D Polar Coordinates (二次元極座標)
documentation_of: ../src/algorithm/geometry/2d/polar.hpp
---

## API

- `polar(radius, theta)`: 長さ `radius`、偏角 `theta` の点を返す。角度はラジアン。

## API別の時間計算量・空間計算量

- `polar`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

負の `radius` も数式どおり計算し、検査しない。
