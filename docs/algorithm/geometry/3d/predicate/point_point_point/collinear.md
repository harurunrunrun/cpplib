---
title: 3D Collinearity (三次元共線判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/point_point_point/collinear.hpp
---

## API

- `collinear(first, second, third)`: 3 点が同一直線上にあるかを判定する。`second - first` と `third - first` が相対許容誤差 `GEOMETRY3D_EPS` の範囲で平行かで判定する。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 一致する点を含む退化した配置も共線と判定する。
- 非有限な入力（差がオーバーフローした場合を含む）には `std::invalid_argument` を送出する。
