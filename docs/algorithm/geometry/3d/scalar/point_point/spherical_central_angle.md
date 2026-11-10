---
title: Spherical Central Angle (球面中心角)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/point_point/spherical_central_angle.hpp
---

## API

- `spherical_central_angle(first, second)`: 原点から見た 2 方向のなす小さい方の中心角を $[0, \pi]$ の範囲（ラジアン）で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 入力は有限な非零ベクトルでなければならず、違反時は `std::invalid_argument` を送出する。
- ベクトルの長さは無視するため、半径の異なる点同士にも使える。
- $\operatorname{atan2}(\lVert a \times b \rVert, a \cdot b)$ を用いるため、同方向・逆方向の近くでも `acos` 単独より数値的に安定である。
