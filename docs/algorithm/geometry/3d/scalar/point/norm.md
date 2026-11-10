---
title: 3D Squared Norm (三次元ノルムの二乗)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/point/norm.hpp
---

## API

- `norm(point)`: ベクトルの長さの二乗 `dot(point, point)` を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 平方根はとらない。長さが必要なら `abs(point)` を使う。
- 入力の検査やオーバーフローの検出は行わない（`dot` と同じ）。
