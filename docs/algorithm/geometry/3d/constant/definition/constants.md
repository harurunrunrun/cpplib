---
title: 3D Geometry Constants (三次元幾何定数)
documentation_of: ../../../../../../src/algorithm/geometry/3d/constant/definition/constants.hpp
---

## API

- `GEOMETRY3D_EPS`: 浮動小数点数の判定に用いる許容誤差 $10^{-10}$（`long double`）。
- `GEOMETRY3D_PI`: `long double` 精度の円周率。

## API別の時間計算量・空間計算量

- 全操作: 時間・追加領域ともに $O(1)$。

## 注意点

許容誤差を絶対誤差として使うか、スケールを掛けた相対誤差として使うか、そもそも使うかどうかは、個別の API の契約に従う。
