---
title: 3D Geometry Sign (三次元幾何の符号判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/integer/scalar/geometry3d_sign.hpp
---

## API

- `geometry3d_sign(value)`: `value` が `GEOMETRY3D_EPS` より大きければ `1`、`-GEOMETRY3D_EPS` より小さければ `-1`、それ以外なら `0` を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

絶対誤差による判定であり、座標の規模に応じた相対誤差ではない。
