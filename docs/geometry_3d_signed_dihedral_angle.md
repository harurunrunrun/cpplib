---
title: Signed 3D Dihedral Angle (三次元符号付き二面角)
documentation_of: ../src/algorithm/geometry/3d/scalar/signed_dihedral_angle.hpp
---

## API

- `signed_dihedral_angle(first, second, oriented_axis)`: 軸の右ねじ方向で第1法線から第2法線への角を $[-\pi,\pi]$ で返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

軸は非零で両平面に平行でなければならない。違反または零法線には `std::invalid_argument` を送出する。
