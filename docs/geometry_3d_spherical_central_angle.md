---
title: Spherical Central Angle (球面中心角)
documentation_of: ../src/algorithm/geometry/3d/scalar/spherical_central_angle.hpp
---

## API

- `spherical_central_angle(first, second)`: 原点から見た2方向の小さい中心角を$[0,\pi]$ラジアンで返す。

## API別の時間計算量・空間計算量

時間$O(1)$、追加領域$O(1)$。

## 注意点

入力は有限な非零ベクトルでなければならない。長さは無視し、異なる半径上の点にも使える。`atan2(|a\times b|,a\cdot b)`を用いるため、同方向・反対方向の近傍でも`acos`単独より安定である。
