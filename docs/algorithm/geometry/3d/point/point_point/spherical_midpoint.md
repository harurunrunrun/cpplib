---
title: Spherical Midpoint (球面中点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/point_point/spherical_midpoint.hpp
---

## API

- `spherical_midpoint(first, second)`: 2 方向を結ぶ短い方の大円弧の中点を、単位ベクトルで返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 入力は有限な非零ベクトルでなければならず、違反時は `std::invalid_argument` を送出する。
- 2 方向が正反対の場合は中点が一意でないため `std::domain_error` を送出する。
- 入力ベクトルの長さは結果に影響しない。
