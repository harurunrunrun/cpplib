---
title: 3D Scalar Triple Product (三次元スカラー三重積)
documentation_of: ../src/algorithm/geometry/3d/scalar_triple.hpp
---

## API

- `scalar_triple(first, second, third)`: `dot(first, cross(second, third))` を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

符号は右手系の向きに従う。
