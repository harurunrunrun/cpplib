---
title: 2D Geometry Constants (二次元幾何定数)
documentation_of: ../src/algorithm/geometry/2d/constants.hpp
---

二次元幾何で共有する誤差定数。

## API

- `GEOMETRY_EPS`: 符号判定や等値判定に用いる `1e-10L`。

## API別の時間計算量・空間計算量

- 定数の参照: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

絶対誤差の基準値である。座標の大きさに応じた相対誤差ではない。
