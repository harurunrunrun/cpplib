---
title: 3D Geometry Sign (三次元幾何の符号判定)
documentation_of: ../src/algorithm/geometry/geometry_3d/geometry3d_sign.hpp
---

## API

- `geometry3d_sign(value)`: `GEOMETRY3D_EPS` による許容誤差付き符号を `-1,0,1` で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

絶対誤差判定であり、座標規模に応じた相対誤差ではない。
