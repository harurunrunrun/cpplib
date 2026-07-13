---
title: 2D Geometry Sign (二次元幾何の符号判定)
documentation_of: ../src/algorithm/geometry/2d/geometry_sign.hpp
---

浮動小数値を許容誤差付きで符号化する。

## API

- `geometry_sign(value)`: `value > GEOMETRY_EPS` なら1、`value < -GEOMETRY_EPS` なら-1、それ以外は0を返す。

## API別の時間計算量・空間計算量

- `geometry_sign`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

判定は `GEOMETRY_EPS` による絶対誤差判定である。
