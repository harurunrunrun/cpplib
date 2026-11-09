---
title: 3D Scalar Triple Product (三次元スカラー三重積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/point_point_point/scalar_triple.hpp
---

## API

- `scalar_triple(first, second, third)`: `dot(first, cross(second, third))`、すなわち 3 ベクトルを並べた行列式を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 3 ベクトルが右手系をなすとき正、左手系のとき負になる。
- 入力の検査やオーバーフローの検出は行わない。
