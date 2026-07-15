---
title: 3D Orthonormal Basis (三次元正規直交基底)
documentation_of: ../src/algorithm/geometry/3d/orthonormal_basis_3d.hpp
---

指定方向を第1軸とする右手系の三次元正規直交基底を構築する。

## API

- `orthonormal_basis_3d(first_axis)`: `(u, v, w)` を返す。`u` は
  `first_axis` と同方向で、`w = u cross v`。

## 時間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 第1軸に最も平行でない座標軸を補助方向に選び、cross productの桁落ちを避ける。
- ゼロまたは非有限な入力には `std::invalid_argument` を送出する。
