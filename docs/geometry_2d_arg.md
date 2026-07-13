---
title: 2D Vector Argument (二次元ベクトルの偏角)
documentation_of: ../src/algorithm/geometry/2d/arg.hpp
---

## API

- `arg(point)`: `atan2(y, x)` による偏角をラジアンで返す。

## API別の時間計算量・空間計算量

- `arg`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

零ベクトルに対する値は実行環境の `atan2(0, 0)` に従う。
