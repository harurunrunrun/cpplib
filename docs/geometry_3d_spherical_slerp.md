---
title: Spherical SLERP (球面線形補間)
documentation_of: ../src/algorithm/geometry/3d/spherical_slerp.hpp
---

## API

- `spherical_slerp(first, second, parameter)`: 2方向間の短い大円弧を`parameter`で補間した単位ベクトルを返す。

## API別の時間計算量・空間計算量

時間$O(1)$、追加領域$O(1)$。

## 注意点

入力は有限な非零ベクトル、`parameter`は有限な$[0,1]$でなければならない。反対方向では経路が一意でないため`std::domain_error`を送出する。入力半径は無視する。
