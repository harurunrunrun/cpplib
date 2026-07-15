---
title: Spherical Midpoint (球面中点)
documentation_of: ../src/algorithm/geometry/3d/spherical_midpoint.hpp
---

## API

- `spherical_midpoint(first, second)`: 2方向を結ぶ短い大円弧の中点を単位ベクトルで返す。

## API別の時間計算量・空間計算量

時間$O(1)$、追加領域$O(1)$。

## 注意点

入力は有限な非零ベクトルでなければならない。反対方向では中点が一意でないため`std::domain_error`を送出する。入力半径は結果に影響しない。
